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

#include "OppServer.h"
#include "Job.h"
#include "SelectionStrategies.h"
#include "IPassiveQueue.h"

namespace queueing {

Define_Module(OppServer);

OppServer::OppServer() {
    // TODO Auto-generated constructor stub
    selectionStrategy = nullptr;
    allocated = false; // true if the server was requested

    jobServiced = nullptr;
    jobInterrupted = nullptr; // don't know if needed

    endServiceMsg = nullptr;
    startSwitchEvent = nullptr; // don't know if needed
    endSwitchOverTimeEvent = nullptr; // don't know if needed

}

OppServer::~OppServer() {
    // TODO Auto-generated destructor stub
    delete selectionStrategy;

    delete jobServiced;
    delete jobInterrupted;

    cancelAndDelete(startSwitchEvent);
    cancelAndDelete(endSwitchOverTimeEvent);
    cancelAndDelete(endServiceMsg);
}

void OppServer::initialize() {

    busySignal = registerSignal("busy");
    emit(busySignal, false);

    allocated = false;
    // really not needed
    selectionStrategy = SelectionStrategy::create(par("fetchingAlgorithm"), this, true);

    jobServiced = nullptr;
    isServingQ1 = par("isServingQ1"); // true at the beginning

    serviceTime = par("serviceTime");
    switchOverTime = par("switchOverTime");
    Q1visitTime = par("Q1visitTime");
    Q2visitTime = par("Q2visitTime");

    endServiceMsg = new cMessage("end-service");
    startSwitchEvent = new cMessage("start-switch-event");
    endSwitchOverTimeEvent = new cMessage("end-switch-over-time-event");
    // at the beginning S is serving Q1
    scheduleAt(simTime()+Q1visitTime, startSwitchEvent);

}

void OppServer::handleMessage(cMessage *msg) {

    // self-messages
    if (msg == startSwitchEvent) {
        // TODO implement later: the server becomes unavailable/idle... does nothing
        if (jobServiced)
         serverIsAvailable = false;
         scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);
    }

    else if (msg == endSwitchOverTimeEvent) {
        // TODO server becomes available again...
        // invert isServingQ1 (from 0 to 1 and viceversa)
        serverIsAvailable = true;
        isServingQ1 = !isServingQ1;
        // EV << "In handleMessage(), after switch isServingQ1 becomes " << isServingQ1 << "\n";

        if (isServingQ1)
           scheduleAt(simTime()+Q1visitTime, startSwitchEvent);
       else
           scheduleAt(simTime()+Q2visitTime, startSwitchEvent);

        // check if serving Q1 or Q2, then request a job
         cGate *inGate = nullptr;

         if (isServingQ1)
             inGate = selectionStrategy->selectableGate(0);
         else
             inGate = selectionStrategy->selectableGate(1);
         // cast the CModule* to IPassiveQueue and in the meantime request a new job
         // this fails if the queue is empty...

         try {
             check_and_cast<IPassiveQueue *>(inGate->getOwnerModule())->request(inGate->getIndex());
         }
         catch(int e) {
             EV << "requesting job from an empty queue";
         }
    }
    else if (msg == endServiceMsg) {
        // ASSERT(jobServiced != nullptr);
        // ASSERT(allocated);
        simtime_t d = simTime() - endServiceMsg->getSendingTime();
        jobServiced->setTotalServiceTime(jobServiced->getTotalServiceTime() + d);

        EV << "deciding where to send the job, isServingQ1 is " << isServingQ1 << "\n";
        if (isServingQ1) {
            // send the job out to Q2
            EV << "Sending the job out to Q2";
            send(jobServiced, "out", 0);
        }
        else {
            // send the job out to Q3
            EV << "Sending the job out to Q3";
            send(jobServiced, "out", 1);
        }

        jobServiced = nullptr;
        allocated = false;
        emit(busySignal, false);

        if (serverIsAvailable) {
            // check if serving Q1 or Q2, then request a job
            cGate *inGate = nullptr;

            if (isServingQ1)
                inGate = selectionStrategy->selectableGate(0);
            else
                inGate = selectionStrategy->selectableGate(1);
            // cast the CModule* to IPassiveQueue and in the meantime request a new job
            // this fails if the queue is empty...

            try {
                check_and_cast<IPassiveQueue *>(inGate->getOwnerModule())->request(inGate->getIndex());
            }
            catch(int e) {
                EV << "requesting job from an empty queue";
            }
        }
    }
    // a new job arrives
    else {
        if (serverIsAvailable) {
            if (!allocated)
                error("job arrived, but the sender did not call allocate() previously");
            if (jobServiced)
            {
                // TODO use here jobInterrupted...
                throw cRuntimeError("a new job arrived while already servicing one");
            }
            // the new job is serviced
            jobServiced = check_and_cast<Job *>(msg);
            simtime_t serviceTime = par("serviceTime");
            scheduleAt(simTime()+serviceTime, endServiceMsg);
            emit(busySignal, true);
        }
        else {
            // send the job back? TODO
        }
    }
}

void OppServer::refreshDisplay() const
{
    getDisplayString().setTagArg("i2", 0, jobServiced ? "status/execute" : "");
}

void OppServer::finish()
{
}

bool OppServer::isIdle()
{
    return !allocated;  // we are idle if nobody has allocated us for processing
}

void OppServer::allocate()
{
    allocated = true;
}

} /* namespace queueing */
