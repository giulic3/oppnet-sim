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
    jobInterruptedQ1 = nullptr; // don't know if needed TODO
    jobInterruptedQ2 = nullptr;

    endServiceMsg = nullptr;
    startSwitchEvent = nullptr;
    endSwitchOverTimeEvent = nullptr;

}

OppServer::~OppServer() {
    // TODO Auto-generated destructor stub
    delete selectionStrategy;

    delete jobServiced;
    delete jobInterruptedQ1;
    delete jobInterruptedQ2;

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

// TODO: implementare meccanismo di ritrasmissione
void OppServer::handleMessage(cMessage *msg) {

    EV << "Message name " << msg->getName() << endl;

    // Self-messages
    if (msg == startSwitchEvent) {
         serverIsAvailable = false;
         // If server is not doing anything (what if it had already requested a job?)
         if (jobServiced) {
             if (isServingQ1) jobInterruptedQ1 = jobServiced;
             else jobInterruptedQ2 = jobServiced;

             jobServiced = nullptr;

         }
         // Schedule next switch in any case
         scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);
         // If jobServiced = true then the server will process the job and the next event
         // will be a end service event
    }
    else if (msg == endSwitchOverTimeEvent) {

        serverIsAvailable = true;
        // Invert isServingQ1 (0 to 1 and 1 to 0)
        isServingQ1 = !isServingQ1;

        if (isServingQ1 && !jobInterruptedQ1) {
            jobServiced = check_and_cast<Job *>(jobInterruptedQ1); // FIX:THIS IS NULL HERE
            scheduleAt(simTime()+serviceTime, endServiceMsg);
            emit(busySignal, true);
        }
        else if (!isServingQ1 && !jobInterruptedQ2) {
            jobServiced = check_and_cast<Job *>(jobInterruptedQ2);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
            emit(busySignal, true);
        }
        else {
            // Schedule next switch event
            if (isServingQ1)
               scheduleAt(simTime()+Q1visitTime, startSwitchEvent);
            else
               scheduleAt(simTime()+Q2visitTime, startSwitchEvent);

            requestJob();
        }
    }
    else if (msg == endServiceMsg) {

        simtime_t d = simTime() - endServiceMsg->getSendingTime();
        jobServiced->setTotalServiceTime(jobServiced->getTotalServiceTime() + d);

        EV << "Deciding where to send the job, isServingQ1 is " << isServingQ1 << "\n";
        if (isServingQ1) {
            // Send the job out to Q2
            EV << "Sending the job out to Q2" << endl;
            send(jobServiced, "out", 0);
            jobInterruptedQ1 = nullptr;

        }
        else {
            // Send the job out to Q3
            EV << "Sending the job out to Q3" << endl;
            send(jobServiced, "out", 1);
            jobInterruptedQ2 = nullptr;

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
        EV << "A new job has arrived. Server is available? " << serverIsAvailable << "\n";
        if (serverIsAvailable) {
            if (!allocated)
                error("job arrived, but the sender did not call allocate() previously");
            if (jobServiced)
            {
                throw cRuntimeError("A new job arrived while already servicing one");
            }
            // The new job is serviced
            jobServiced = check_and_cast<Job *>(msg);
            scheduleAt(simTime()+serviceTime, endServiceMsg);
            emit(busySignal, true);
        }
        else {
            // Send the job back? TODO
        }
    }
}

void OppServer::refreshDisplay() const
{
    getDisplayString().setTagArg("i2", 0, jobServiced ? "status/execute" : "");
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
