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
        simtime_t switchOverTime;
        simtime_t visitTime1; // time at L1
        simtime_t visitTime2; // time at L2

        cMessage *switchToL1Event;
        cMessage *switchToL2Event;
        cMessage *endSwitchTimeEvent; // TODO: not sure

        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
    };

#endif /* OPPQUEUE_H_ */
