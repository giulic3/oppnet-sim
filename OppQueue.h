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

#ifndef OPPQUEUE_H_
#define OPPQUEUE_H_

#include <omnetpp.h>
#include "Queue.h"

using namespace omnetpp;

/** Queue module to model Q2 behavior */

class OppQueue : public queueing::Queue {

    protected:
        // visit time distribution at current location, @L1 or @L2
        simtime_t visitTime;
        simtime_t switchOverTime;

        cMessage *startSwitchEvent;
        cMessage *endSwitchOverTimeEvent;

        bool serverIsAvailable; // true if Q2 @ this location
        bool serverIsIdle; // true if server associated is not processing any jobs
        bool isQ2LastLocation; // true if Q2 was here and must switch

        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
    };

#endif /* OPPQUEUE_H_ */
