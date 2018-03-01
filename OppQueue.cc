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

Define_Module(OppQueue);

void OppQueue::initialize() {

    Queue::initialize();
    // parameter initialization is done in .ini
    // TODO: parameters in this class or for the generic sim?
    switchOverTime = par("switchOverTime");
    visitTime1 = par("visitTimeL1"); // time at L1
    visitTime2 = par("visitTimeL2"); // time at L2

    // when simulation starts, Q2 @ L1
    serverIsUp = false; // true if server S2 is up
    scheduleAt(simTime()+visitTime1, switchToL2Event);
    switchToL2 = true;

}

void OppQueue::handleMessage(cMessage *msg) {

    // self-messages
    if (msg == switchToL2Event) {
        serverIsUp = false;
        // start switchOverTime timer
        scheduleAt(simTime()+switchOverTime, endSwitchTimeEvent);
    }
    else if (msg == switchToL1Event) {
        serverIsUp = false;
        scheduleAt(simTime()+switchOverTime, endSwitchTimeEvent);

    }
    else if (msg == endSwitchTimeEvent) {
        // TODO: wake S2 or S1 depending on switch direction
        if (switchToL2 == true) {// if switch to L2
        serverIsUp = true;
        scheduleAt(simTime()+visitTime2, switchToL1Event);
        }
        else {
            // else if switch to L1
            serverIsUp = false;
            scheduleAt(simTime()+visitTime1, switchToL2Event);
            // TODO: must activate server S1
            cMessage *wakeUpServerEvent = new cMessage("wakeUp");
            send(wakeUpServerEvent, "in"); // TODO: add connection?
            // send(,wakeUpServerEvent);
        }

    }
    // messages from Q1/S1
    else {
        if (serverIsUp) {
            Queue::handleMessage(msg);
        }
        else { // server is down
            // TODO: message is received and not processed, stays in queue
            // until server activation
            EV << "Server is down! Job in queue waiting to be processed\n";
            queue.insert(job);
            emit(queueLengthSignal, length());
            job->setQueueCount(job->getQueueCount() + 1);
        }
    }
}

void OppQueue::refreshDisplay() const {

}

