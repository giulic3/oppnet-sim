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
    // given that RNG is deterministic, all parameter sequences are the same (if the seeds are too)
    visitTime = par("visitTime");
    switchOverTime = par("switchOverTime");

    serverIsAvailable = false;
    // when simulation starts, Q2 @ L1
    isQ2LastLocation = false; // TODO: set true for L1 in .ned file
    scheduleAt(simTime()+visitTime, startSwitchEvent);

}

void OppQueue::handleMessage(cMessage *msg) {


    // self-messages
    if (msg == startSwitchEvent) {
        // TODO if server is processing a job now, the process must be interrupted
        // ...
        serverIsAvailable = false;
        // start switchOverTime timer
        scheduleAt(simTime()+switchOverTime, endSwitchOverTimeEvent);
    }
    else if (msg == endSwitchOverTimeEvent) { // TODO refactor code in elegant way
        if (isQ2LastLocation == true) {
            // if true, Q2 was here and now's gone
            // so server here stays unavailable

        }
        else { // Q2 was not here, and now it is at the end of the switch
            serverIsAvailable = true;
        }

        scheduleAt(simTime()+visitTime, startSwitchEvent);
    }
    // messages from Q1/S1
    else {
        if (serverIsAvailable) {
            Queue::handleMessage(msg);
        }
        else { // server is not available
               // so message can't be received
               // TODO need an ack...
        }
    }
}

void OppQueue::refreshDisplay() const {

}

