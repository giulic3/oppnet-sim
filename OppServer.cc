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

    selectionStrategy = nullptr;
    allocated = false; // true if the server was requested

    jobServiced = nullptr;

    endServiceMsg = nullptr;
    startSwitchEvent = nullptr;
    endSwitchOverTimeEvent = nullptr;

}

OppServer::~OppServer() {

    delete selectionStrategy;
    delete jobServiced;

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
    isServingQ1 = par("isServingQ1"); // True at the beginning

    serviceTime = par("serviceTime");
    switchOverTime = par("switchOverTime");
    Q1visitTime = par("Q1visitTime");
    Q2visitTime = par("Q2visitTime");

    endServiceMsg = new cMessage("end-service");
    startSwitchEvent = new cMessage("start-switch-event");
    endSwitchOverTimeEvent = new cMessage("end-switch-over-time-event");
    scheduleAt(simTime()+Q1visitTime, startSwitchEvent); // ATB Q1 is served

}

void OppServer::handleMessage(cMessage *msg) {

    EV << "Message name " << msg->getName() << endl;

    // Self-messages
    if (msg == startSwitchEvent) {
         serverIsAvailable = false;
         // If server is not doing anything
         if (!jobServiced)
             scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);
         // If jobServiced = true then the server will process the job and the next event
         // will be a end service event
    }
    else if (msg == endSwitchOverTimeEvent) {
        // Invert isServingQ1 (0 to 1 and 1 to 0)
        serverIsAvailable = true;
        isServingQ1 = !isServingQ1;

        // Schedule next switch event
        if (isServingQ1)
           scheduleAt(simTime()+Q1visitTime, startSwitchEvent);
       else
           scheduleAt(simTime()+Q2visitTime, startSwitchEvent);

        requestJob();
    }
    else if (msg == endServiceMsg) {

        simtime_t d = simTime() - endServiceMsg->getSendingTime();
        jobServiced->setTotalServiceTime(jobServiced->getTotalServiceTime() + d);

        EV << "Deciding where to send the job, isServingQ1 is " << isServingQ1 << "\n";
        if (isServingQ1) {
            // Send the job out to Q2
            EV << "Sending the job out to Q2" << endl;
            send(jobServiced, "out", 0);
        }
        else {
            // Send the job out to Q3
            EV << "Sending the job out to Q3" << endl;
            send(jobServiced, "out", 1);
        }

        jobServiced = nullptr;
        allocated = false;
        emit(busySignal, false);

        if (serverIsAvailable) {
            requestJob();
        }
        else
            // If the server is not available then it means it is ready to switch
            scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);

    }
    // A new job arrives
    else {
        // Can happen only upon server request, this means it was available
            if (!allocated)
                error("Job arrived, but the sender did not call allocate() previously");
            if (jobServiced)
                throw cRuntimeError("A new job arrived while already servicing one");

            // The new job is serviced
            jobServiced = check_and_cast<Job *>(msg);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
            emit(busySignal, true);
    }
}

void OppServer::refreshDisplay() const
{
    getDisplayString().setTagArg("i2", 0, jobServiced ? "status/execute" : "");
    // Change icon server color when isServingQ1(red)or Q2(green)
    getDisplayString().setTagArg("i", 1, isServingQ1 ? "red" : "green");
}

void OppServer::finish() {}

// Function that requests a job from a passive queue (Q1 or Q2)
void OppServer::requestJob(){
    // Check if serving Q1 or Q2, then request a job
    cGate *inGate = nullptr;

    if (isServingQ1)
        inGate = selectionStrategy->selectableGate(0);
    else {
        EV << "endServiceMsg: in else" << endl;
        inGate = selectionStrategy->selectableGate(1);
    }
    try {
        EV << "Requesting a job, gate, getOwnerModule: " << inGate << " " << inGate->getOwnerModule() << endl;
        check_and_cast<IPassiveQueue *>(inGate->getOwnerModule())->request(inGate->getIndex());
    }
    catch(int e) {
        EV << "Requesting job from an empty queue";
    }
}

bool OppServer::isIdle()
{
    return !allocated;  // Idle if nobody allocated us for processing
}

void OppServer::allocate()
{
    allocated = true;
}

} /* namespace queueing */
