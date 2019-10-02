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

#ifndef OPPPASSIVEQUEUE_H_
#define OPPPASSIVEQUEUE_H_

#include "QueueingDefs.h"
#include "IPassiveQueue.h"
#include "SelectionStrategies.h"
#include "Job.h"

namespace queueing {


class OppPassiveQueue : public cSimpleModule, public IPassiveQueue {

private:
    simsignal_t droppedSignal;
    simsignal_t queueLengthSignal;
    simsignal_t queueingTimeSignal;

    bool fifo;
    int capacity;
    cQueue queue;
    SelectionStrategy *selectionStrategy;
    Job *savedJob;

    void queueLengthChanged();
    void sendJob(Job *job, int gateIndex);

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

public:
    OppPassiveQueue();
    virtual ~OppPassiveQueue();
    // The following methods are called from IServer:
    virtual int length() override;
    virtual void request(int gateIndex) override;
};

} /* namespace queueing */

#endif /* OPPPASSIVEQUEUE_H_ */
