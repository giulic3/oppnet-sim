//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "OppQueue.h"
#include "Job.h"

namespace queueing {

Define_Module(OppQueue);

OppQueue::OppQueue() {

    jobServiced = nullptr;
    endServiceMsg = nullptr;
    startSwitchEvent = nullptr;
}

OppQueue::~OppQueue() {

    delete jobServiced;
    cancelAndDelete(endServiceMsg);
}

void OppQueue::initialize() {

    droppedSignal = registerSignal("dropped");
    queueingTimeSignal = registerSignal("queueingTime");
    queueLengthSignal = registerSignal("queueLength");
    emit(queueLengthSignal, 0);
    busySignal = registerSignal("busy");
    emit(busySignal, false);

    endServiceMsg = new cMessage("end-service");
    fifo = par("fifo");
    capacity = par("capacity");
    queue.setName("queue");

    // given that RNG is deterministic, all parameter sequences are the same (if the seeds are too)
    visitTime = par("visitTime");
    switchOverTime = par("switchOverTime");
    serverIsAvailable = par("serverIsAvailable"); // must be true for Q1 at the beginning
    serverIsIdle = true; // TODO forse superflua
    // when simulation starts, Q2 @ L1
    isQ2LastLocation = par("isQ2LastLocation"); // true for L1 and false for L2 at the beginning

    startSwitchEvent = new cMessage("start-switch-event");
    endSwitchOverTimeEvent = new cMessage("start-switch-over-time-event");
    scheduleAt(simTime()+visitTime, startSwitchEvent);
}

// TODO il problema è che gli eventi non sono concorrenti tra due code diverse,
// per cui il mio modello non funziona
void OppQueue::handleMessage(cMessage *msg) {
    // self-messages
    if (msg == startSwitchEvent) {
        // if the server is processing a job now (is not idle),
        // the process must be interrupted
        if (serverIsIdle == false) {
            // take current job and enqueue to process it next time
            Job *job = jobServiced;
            queue.insert(job);
            emit(queueLengthSignal, length());
            job->setQueueCount(job->getQueueCount() + 1);
            // TODO not sure, null pointer exception?
            jobServiced = nullptr;
        }
        serverIsAvailable = false;
        EV << "serverIsAvailable = false" << endl;
        // start switchOverTime timer, when it ends switch has completed
        scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);
    }
    else if (msg == endSwitchOverTimeEvent) {
        if (isQ2LastLocation == true) {
            // if true, Q2 was here and now's gone
            // so the server here stays unavailable
        }
        else { // Q2 was not here, and now it is at the end of the switch
            serverIsAvailable = true;
            EV << "serverIsAvailable = true" << endl;
            // TODO process all the jobs that were interrupted or arrived when the server was unavailable
            // forse non serve, viene fatto in automatico
            if (queue.isEmpty()) {
                jobServiced = nullptr;
                serverIsIdle = true;
                emit(busySignal, false);
            }
            else {
                jobServiced = getFromQueue();
                emit(queueLengthSignal, length());
                simtime_t serviceTime = startService(jobServiced);
                scheduleAt(simTime()+serviceTime, endServiceMsg);
            }
            //
        scheduleAt(simTime()+visitTime, startSwitchEvent);
        }
    }
    else {
        if (serverIsAvailable) { // means Q2 is @ current location

            // other messages
            if (msg == endServiceMsg) {
                endService(jobServiced);
                if (queue.isEmpty()) {
                    jobServiced = nullptr;
                    serverIsIdle = true; // TODO superflua se si usa jobServiced
                    emit(busySignal, false);
                }
                else {
                    jobServiced = getFromQueue();
                    emit(queueLengthSignal, length());
                    simtime_t serviceTime = startService(jobServiced);
                    scheduleAt(simTime()+serviceTime, endServiceMsg);
                }
            }
            else { // a message of another type has arrived
                EV << "generic message/job" << endl;
                serverIsIdle = false;

                Job *job = check_and_cast<Job *>(msg);
                arrival(job);

                if (!jobServiced) {
                    // processor was idle, not processing any job
                    jobServiced = job;
                    emit(busySignal, true);
                    EV << "start service" << endl;
                    simtime_t serviceTime = startService(jobServiced);
                    scheduleAt(simTime()+serviceTime, endServiceMsg);
                }
                else {
                    // check for container capacity, useless if queue is unlimited
                    if (capacity >= 0 && queue.getLength() >= capacity) {
                        EV << "Capacity full! Job dropped.\n";
                        if (hasGUI())
                            bubble("Dropped!");
                        emit(droppedSignal, 1);
                        delete job;
                        return;
                    }
                    queue.insert(job);
                    emit(queueLengthSignal, length());
                    job->setQueueCount(job->getQueueCount() + 1);
                }
                //serverIsIdle = true; // TODO not sure
            }
        }
        // if server is not available
        // TODO message is enqueued but will be processed
        // only when the next handleMessage is called
        else { // cosa succede quando arriva un endservicemsg ma il server non è available?
            Job *job = check_and_cast<Job *>(msg);
            arrival(job);//
            this->queue.insert(job);
            emit(queueLengthSignal, length());
            job->setQueueCount(job->getQueueCount() + 1);
        }
    }

}

void OppQueue::refreshDisplay() const {
    getDisplayString().setTagArg("i2", 0, jobServiced ? "status/execute" : "");
    getDisplayString().setTagArg("i", 1, queue.isEmpty() ? "" : "cyan");
}

Job *OppQueue::getFromQueue() {
    Job *job;
    if (fifo) {
        job = (Job *)queue.pop();
    }
    else {
        job = (Job *)queue.back();
        // FIXME this may have bad performance as remove uses linear search
        queue.remove(job);
    }
    return job;
}

int OppQueue::length() {
    return queue.getLength();
}

void OppQueue::arrival(Job *job) {
    job->setTimestamp();
}

simtime_t OppQueue::startService(Job *job) {
    // gather queueing time statistics
    simtime_t d = simTime() - job->getTimestamp();
    emit(queueingTimeSignal, d);
    job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
    EV << "Starting service of " << job->getName() << endl;
    job->setTimestamp();
    return par("serviceTime").doubleValue();
}

void OppQueue::endService(Job *job) {
    EV << "Finishing service of " << job->getName() << endl;
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalServiceTime(job->getTotalServiceTime() + d);
    send(job, "out");
}

void OppQueue::finish() {
}

}; // namespace
