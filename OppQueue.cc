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

#define PRIORITY_GATE 0
#define NORMAL_GATE 1

namespace queueing {

    Define_Module(OppQueue);

    OppQueue::OppQueue() {

        jobServiced = nullptr;
        jobInterrupted = nullptr;
        jobInterruptedTime = 0.;
        jobInterruptedSimTime = 0.;

        endServiceMsg = nullptr;
        startSwitchEvent = nullptr;
        endSwitchOverTimeEvent = nullptr;
        //wakeUpServerEvent = nullptr;
    }

    OppQueue::~OppQueue() {

        delete jobServiced;
        delete jobInterrupted;

        // prevent "undisposed object" warning
        cancelAndDelete(endServiceMsg);
        cancelAndDelete(startSwitchEvent);
        cancelAndDelete(endSwitchOverTimeEvent);
        //cancelAndDelete(wakeUpServerEvent);

    }

    void OppQueue::initialize() {

        droppedSignal = registerSignal("dropped");
        // time spent in the queue (excluded the time needed by the server to process it)
        queueingTimeSignal = registerSignal("queueingTime");
        queueLengthSignal = registerSignal("queueLength");
        emit(queueLengthSignal, 0);
        busySignal = registerSignal("busy");
        emit(busySignal, false);

        fifo = par("fifo");
        capacity = par("capacity");
        queue.setName("queue");

        visitTime = par("visitTime");
        switchOverTime = par("switchOverTime");
        serverIsAvailable = par("serverIsAvailable"); // must be true for Q1 at the beginning, set in the simulation configuration

        endServiceMsg = new cMessage("end-service");
        startSwitchEvent = new cMessage("start-switch-event");
        endSwitchOverTimeEvent = new cMessage("end-switch-over-time-event");
        //wakeUpServerEvent = new cMessage("wake-up-server-event");
        // fire the timer for S1/Q1
        if (serverIsAvailable)
            scheduleAt(simTime()+visitTime, startSwitchEvent);
    }

    void OppQueue::handleMessage(cMessage *msg) {

        // EV << "Message name " << msg->getName() << endl;
        // self-messages
        if (msg == startSwitchEvent) {
            // if the server is processing a job now, it must be interrupted
            if (jobServiced) {
                EV << "Job interrupted! It will be processed next time" << endl;
                // take current job and save it temporarily to process it the next time the server becomes available
                jobInterrupted = jobServiced;
                //Job *job = jobServiced;
                //queue.insert(job);
                // put the interrupted job among the pending ones
                emit(queueLengthSignal, length()+1);
                jobInterrupted->setQueueCount(jobInterrupted->getQueueCount() + 1);
                jobServiced = nullptr;

            }
            serverIsAvailable = false;
            cancelEvent(endServiceMsg);
            EV << "serverIsAvailable = false" << endl;

            // start switchOverTime timer, when it ends switch has completed
            scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);
        }

        else if (msg == endSwitchOverTimeEvent) {
            // wake up the other queue
            cMessage *wakeUpServerEvent = new cMessage("wake-up-server-event");
            cGate *out = gate("out", PRIORITY_GATE);
            send(wakeUpServerEvent, out);
        }

        else if (strcmp(msg->getName(),"wake-up-server-event") == 0) {

            //EV << "In wake up server event" << endl;
            serverIsAvailable = true;

            // first process the job that was interrupted (if any)
            if (jobInterrupted) {
                jobServiced = jobInterrupted;
                jobInterrupted = nullptr;
                emit(queueLengthSignal, length());
                // from the serviceTime it is subtracted the time already spent in the queue
                simtime_t serviceTime = startService(jobServiced);
                EV << "jobInterrupted serviceTime: " << serviceTime << endl;

                EV << "endServiceMsg scheduled at: " << simTime() + serviceTime << endl;
                scheduleAt(simTime()+serviceTime, endServiceMsg);
            }

            else if (queue.isEmpty()) {
                jobServiced = nullptr;
                emit(busySignal, false);
            }

            else {
                // process all the jobs that arrived when the server was unavailable
                jobServiced = getFromQueue();
                emit(queueLengthSignal, length());
                simtime_t serviceTime = startService(jobServiced);
                scheduleAt(simTime()+serviceTime, endServiceMsg);
            }

            // this deletes the wakeUpServerEvent initialized in the other queue
            cancelAndDelete(msg);
            scheduleAt(simTime()+visitTime, startSwitchEvent);
        }

        else {
            if (serverIsAvailable) { // means Q2 is @ current location
                // other messages
                if (msg == endServiceMsg) {
                    // EV << "End service msg" << endl;
                    endService(jobServiced, NORMAL_GATE);
                    if (queue.isEmpty()) {
                        jobServiced = nullptr;
                        emit(busySignal, false);
                    }
                    else {
                        jobServiced = getFromQueue();
                        emit(queueLengthSignal, length());
                        simtime_t serviceTime = startService(jobServiced);
                        scheduleAt(simTime()+serviceTime, endServiceMsg);
                    }
                }
                else { // a generic message or job has arrived
                    EV << "Generic message/job" << endl;
                    Job *job = check_and_cast<Job *>(msg);
                    arrival(job);

                    if (!jobServiced) {
                        // processor was idle, not processing any jobs
                        jobServiced = job;
                        emit(busySignal, true);
                        EV << "Start service" << endl;
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
            // if the server is not available, the job is enqueued and will be processed next time
            else {

                if (strcmp(msg->getName(),"end-service") != 0) {
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

        //jobInterruptedTime = job->getTimestamp();
        //EV << "(Before) jobInterruptedTime: " << jobInterruptedTime << endl;
        //EV << "(Before) timeStamp(): " << job->getTimestamp() << endl;

        // gather queuing time statistics
        simtime_t d = simTime() - job->getTimestamp();
        emit(queueingTimeSignal, d);
        job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
        EV << "Starting service of " << job->getName() << endl;
        job->setTimestamp();

        //jobInterruptedTime = job->getTimestamp() - jobInterruptedTime;
        //jobInterruptedSimTime = simTime();
        //EV << "jobInterruptedSimTime: " << jobInterruptedSimTime << endl;
        //EV << "(After) jobInterruptedTime: " << jobInterruptedTime << endl;
        //EV << "(After) timeStamp(): " << job->getTimestamp() << endl;

        return par("serviceTime").doubleValue();
    }

    void OppQueue::endService(Job *job, int gateIndex) {
        EV << "Finishing service of " << job->getName() << endl;
        // measuring job service time
        simtime_t d = simTime() - job->getTimestamp();
        job->setTotalServiceTime(job->getTotalServiceTime() + d);
        cGate *out = gate("out", gateIndex);
        send(job, out);
    }

    void OppQueue::finish() {
    }

}; // namespace
