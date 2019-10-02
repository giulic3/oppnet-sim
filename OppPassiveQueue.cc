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

/*
 * */

#include "OppPassiveQueue.h"
#include "Job.h"
#include "IServer.h"

namespace queueing {

Define_Module(OppPassiveQueue)

OppPassiveQueue::OppPassiveQueue() {
    // TODO Auto-generated constructor stub
    selectionStrategy = nullptr;
    savedJob = nullptr;
}

OppPassiveQueue::~OppPassiveQueue() {
    // TODO Auto-generated destructor stub
    delete selectionStrategy;
}

void OppPassiveQueue::initialize() {
    droppedSignal = registerSignal("dropped");
    queueingTimeSignal = registerSignal("queueingTime");
    queueLengthSignal = registerSignal("queueLength");
    emit(queueLengthSignal, 0);

    capacity = par("capacity");
    queue.setName("queue");

    fifo = par("fifo");

    selectionStrategy = SelectionStrategy::create(par("sendingAlgorithm"), this, false);
    if (!selectionStrategy)
        throw cRuntimeError("invalid selection strategy");

}

void OppPassiveQueue::handleMessage(cMessage *msg) {
    EV << "Message name " << msg->getName() << endl;
    // If acknowledgment from server arrived, delete original job
    if (strcmp(msg->getName(), "ack") == 0) {
        EV << "Ack received. Removing original job.\n";
        //job = (Job *)queue.back();
        savedJob = (Job *)queue.remove(savedJob);
        delete savedJob;
        savedJob = nullptr;
        emit(queueLengthSignal, length());
    }
    // If new job arrived
    else {
        Job *job = check_and_cast<Job *>(msg);
        job->setTimestamp();

        // Check for container capacity (just for queues with limited capacity)
        if (capacity >= 0 && queue.getLength() >= capacity) {
            EV << "Queue full! Job dropped.\n";
            if (hasGUI())
                bubble("Dropped!");
            emit(droppedSignal, 1);
            delete msg;
            return;
        }

        // Always enqueue, the packet in queue has to wait for a server request
        queue.insert(job);
        emit(queueLengthSignal, length());
        job->setQueueCount(job->getQueueCount() + 1);
    }
}

void OppPassiveQueue::refreshDisplay() const {
    // Change the icon color
    getDisplayString().setTagArg("i", 1, queue.isEmpty() ? "" : "cyan");
}

int OppPassiveQueue::length() {
    return queue.getLength();
}

void OppPassiveQueue::request(int gateIndex) {

    if (!queue.isEmpty()) {
        Enter_Method("request()!");

        //ASSERT(!queue.isEmpty());

        Job *job;
        // This queue always works as fifo
        if (fifo) {
            // Returns pointer to the object at the front of the queue - the el returned by pop()
            job = (Job *)queue.front();
        }
        else {
            job = (Job *)queue.back();
            // FIXME this may have bad performance as remove uses linear search
            queue.remove(job);
        }


        job->setQueueCount(job->getQueueCount()+1);
        simtime_t d = simTime() - job->getTimestamp();
        job->setTotalQueueingTime(job->getTotalQueueingTime() + d);
        emit(queueingTimeSignal, d);

        // Send copy of job - necessary to implement retransmission
        Job *copy = (Job *)job->dup();
        sendJob(copy, gateIndex);
        // Save pointer to current job to remove later if transmission was successful
        savedJob = job;
    }
    else {
        EV << "The server requested a job, but this queue is empty..." << endl;
        return;
    }
}

void OppPassiveQueue::sendJob(Job *job, int gateIndex) {
    cGate *out = gate("out", gateIndex);
    send(job, out);
    check_and_cast<IServer *>(out->getPathEndGate()->getOwnerModule())->allocate();
}

} /* namespace queueing */
