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

#ifndef NEWOPPQUEUE_H_
#define NEWOPPQUEUE_H_

#include "QueueingDefs.h"

namespace queueing {

class Job;

class NewOppQueue: public cSimpleModule {

private:
    simsignal_t droppedSignal;
    simsignal_t queueLengthSignal;
    simsignal_t queueingTimeSignal;
    simsignal_t busySignal;

    Job *jobServiced;
    cMessage *endServiceMsg;
    cQueue queue;
    int capacity;
    bool fifo;

    Job *getFromQueue();

public:
    NewOppQueue();
    virtual ~NewOppQueue();
    int length();

    // TODO: add getters/setters

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void finish() override;

    // hook functions to (re)define behaviour
    virtual void arrival(Job *job);
    virtual simtime_t startService(Job *job);
    virtual void endService(Job *job);
};
}
;
//namespace

#endif /* NEWOPPQUEUE_H_ */

