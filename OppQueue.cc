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

#define PRIORITY_GATE 0 //TODO sono necessari due gate?
#define NORMAL_GATE 1

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

    visitTime = par("visitTime");
    switchOverTime = par("switchOverTime");
    serverIsAvailable = par("serverIsAvailable"); // must be true for Q1 at the beginning
    //serverIsIdle = true;

    startSwitchEvent = new cMessage("start-switch-event");
    endSwitchOverTimeEvent = new cMessage("start-switch-over-time-event");
    wakeUpServerEvent = new cMessage("wake-up-server-event");

    if (serverIsAvailable)
        scheduleAt(simTime()+visitTime, startSwitchEvent);
}

void OppQueue::handleMessage(cMessage *msg) {

    EV << "nome del messaggio " << msg->getName() << endl;
    // self-messages
    if (msg == startSwitchEvent) {
        // if the server is processing a job now, it must be interrupted
        //if (serverIsIdle == false) {
        if (jobServiced) {
            EV << "job interrupted! it will be processed next time" << endl;
            // take current job and enqueue to process it next time
            Job *job = jobServiced;
            queue.insert(job);
            emit(queueLengthSignal, length());
            job->setQueueCount(job->getQueueCount() + 1);
            jobServiced = nullptr;
            //serverIsIdle = true;

        }
        serverIsAvailable = false;
        cancelEvent(endServiceMsg);
        EV << "serverIsAvailable = false" << endl;
        // start switchOverTime timer, when it ends switch has completed
        scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);
    }
    else if (msg == endSwitchOverTimeEvent) {
        // wake up the other queue
        // TODO memory leak? chi li cancella?
        cMessage *wakeUpServerEvent = new cMessage("wake-up-server-event");
        cGate *out = gate("out", PRIORITY_GATE);
        send(wakeUpServerEvent, out);
        //...
     }
    else if (strcmp(msg->getName(),"wake-up-server-event") == 0) {
        EV << "in wake up server event" << endl;
        serverIsAvailable = true;
        if (queue.isEmpty()) {
            jobServiced = nullptr;
            //serverIsIdle = true;
            emit(busySignal, false);
        }
        else {
            // process all the jobs that were interrupted or arrived
            // when the server was unavailable
            jobServiced = getFromQueue();
            //serverIsIdle = false;
            emit(queueLengthSignal, length());
            simtime_t serviceTime = startService(jobServiced);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
        }
    cancelEvent(wakeUpServerEvent);
    scheduleAt(simTime()+visitTime, startSwitchEvent);
    }
    else {
        if (serverIsAvailable) { // means Q2 is @ current location
            // other messages
            if (msg == endServiceMsg) {
                EV << "end service msg" << endl;
                endService(jobServiced, NORMAL_GATE);
                if (queue.isEmpty()) {
                    jobServiced = nullptr;
                    //serverIsIdle = true;
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
                //serverIsIdle = false;
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
                    // check for container capacity if queue has a size)
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
            }
        }
        // if the server is not available, msg is enqueued but will be processed next time
        else { // TODO cosa succede quando arriva un endservicemsg ma il server non è available?

            if (strcmp(msg->getName(),"end-service") != 0) {
                EV << "casting 2? " << endl;
                Job *job = check_and_cast<Job *>(msg);
                arrival(job);//
                this->queue.insert(job);
                emit(queueLengthSignal, length());
                job->setQueueCount(job->getQueueCount() + 1);
            }
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

void OppQueue::endService(Job *job, int gateIndex) {
    EV << "Finishing service of " << job->getName() << endl;
    simtime_t d = simTime() - job->getTimestamp();
    job->setTotalServiceTime(job->getTotalServiceTime() + d);
    cGate *out = gate("out", gateIndex);
    send(job, out);
}

void OppQueue::finish() {
}

}; // namespace
