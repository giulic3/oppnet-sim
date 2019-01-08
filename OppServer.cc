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
    startSwitchEvent = nullptr;
    endSwitchOverTimeEvent = nullptr;

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
         serverIsAvailable = false;
         // if server is not doing anything (what if it had already requested a job?)
         if (!jobServiced)
             scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);

    }
    else if (msg == endSwitchOverTimeEvent) {
        // invert isServingQ1 (0 to 1 and 1 to 0)
        serverIsAvailable = true;
        isServingQ1 = !isServingQ1;

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

        try {
            // if (!jobServiced)
            EV << "requesting a job, gate index: " << inGate->getIndex() << endl;
                check_and_cast<IPassiveQueue *>(inGate->getOwnerModule())->request(inGate->getIndex());
        }
        catch(int e) {
            EV << "requesting job from an empty queue";
        }
    }
    else if (msg == endServiceMsg) {

        simtime_t d = simTime() - endServiceMsg->getSendingTime();
        jobServiced->setTotalServiceTime(jobServiced->getTotalServiceTime() + d);

        EV << "deciding where to send the job, isServingQ1 is " << isServingQ1 << "\n";
        if (isServingQ1) {
            // send the job out to Q2
            EV << "Sending the job out to Q2" << endl;
            send(jobServiced, "out", 0);
        }
        else {
            // send the job out to Q3
            EV << "Sending the job out to Q3" << endl;
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

            try {
                EV << "requesting a job, gate index: " << inGate->getIndex() << endl;
                check_and_cast<IPassiveQueue *>(inGate->getOwnerModule())->request(inGate->getIndex());
            }
            catch(int e) {
                EV << "requesting job from an empty queue";
            }
        }
        else
            // if the server is not available means it is ready to switch
            scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);

    }
    // a new job arrives
    else {
        EV << "A new job has arrived. Server is available? " << serverIsAvailable << "\n";
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
