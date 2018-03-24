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
    serverIsAvailable = false;
    serverIsIdle = true;
    // when simulation starts, Q2 @ L1
    isQ2LastLocation = false; // TODO: set true for L1 in .ned file

    startSwitchEvent = new cMessage("start-switch-event");
    scheduleAt(simTime()+visitTime, startSwitchEvent);
}

void OppQueue::handleMessage(cMessage *msg) {
    if (msg == endServiceMsg) {
        endService(jobServiced);
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
    else {
        Job *job = check_and_cast<Job *>(msg);
        arrival(job);

        if (!jobServiced) {
            // processor was idle
            jobServiced = job;
            emit(busySignal, true);
            simtime_t serviceTime = startService(jobServiced);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
        }
        else {
            // check for container capacity
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


/**
 *
 * void OppQueue::handleMessage(cMessage *msg) {
-    // self-messages
-    if (msg == startSwitchEvent) {
-        // TODO if server is processing a job now, the process must be interrupted
-        // check if server is idle
-        if (serverIsIdle == false) {
-            //...
-        }
-        serverIsAvailable = false;
-        // start switchOverTime timer
-        scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);
-    }
-    else if (msg == endSwitchOverTimeEvent) { // TODO refactor code in elegant way
-        if (isQ2LastLocation == true) {
-            // if true, Q2 was here and now's gone
-            // so server here stays unavailable
-        }
-        else { // Q2 was not here, and now it is at the end of the switch
-            serverIsAvailable = true;
-        }
-        scheduleAt(simTime()+visitTime, startSwitchEvent);
-    }
-    // messages from Q1/S1
-    else {
-        if (serverIsAvailable) {
-            serverIsIdle = false;
-            Queue::handleMessage(msg);
-            serverIsIdle = true; // TODO not sure
-        }
-        // server is not available
-        // TODO message is enqueued but will be processed
-        // only when the next handleMessage is called
-        else {
-            Job *job = check_and_cast<Job *>(msg);
-            arrival(job);//
-            this->queue.insert(job);
-            emit(queueLengthSignal, length());
-            job->setQueueCount(job->getQueueCount() + 1);
-            */
-       /* }
-    }
-}
 *
 **/
