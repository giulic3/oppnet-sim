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

#ifndef OPPSERVER_H_
#define OPPSERVER_H_

#include "IServer.h"

namespace queueing {

class Job;
class SelectionStrategy;

class OppServer : public cSimpleModule, public IServer{

private:
    simsignal_t busySignal;
    bool allocated;
    bool isServingQ1;
    bool serverIsAvailable;

    SelectionStrategy *selectionStrategy;

    simtime_t serviceTime;
    // time to switch from one position/queue to the other
    simtime_t switchOverTime;
    simtime_t Q1visitTime;
    simtime_t Q2visitTime;

    Job *jobServiced;
    Job *jobInterrupted;
    cMessage *endServiceMsg;
    cMessage *startSwitchEvent;
    cMessage *endSwitchOverTimeEvent;


public:
    OppServer();
    virtual ~OppServer();

protected:
    virtual void initialize() override;
    virtual int numInitStages() const override {return 2;}
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void finish() override;


public:
    virtual bool isIdle() override;
    virtual void allocate() override;
};

} /* namespace queueing */

#endif /* OPPSERVER_H_ */
