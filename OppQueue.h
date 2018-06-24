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

#include "QueueingDefs.h"

//using namespace omnetpp;

namespace queueing {

class Job;

class OppQueue: public cSimpleModule {

private:
    simsignal_t droppedSignal;
    simsignal_t queueLengthSignal;
    simsignal_t queueingTimeSignal;
    simsignal_t busySignal;
    // simsignal_t servedUsers;
    // counter for total number of users served
    // (how many packets arrived in the sink) look SinkExt


    Job *jobServiced;
    cMessage *endServiceMsg;
    cQueue queue;
    int capacity;
    bool fifo;

    Job *getFromQueue();

    // visit time distribution at current location, @L1 or @L2
    simtime_t visitTime;
    // time to switch from one position to the other
    simtime_t switchOverTime;

    cMessage *startSwitchEvent;
    cMessage *endSwitchOverTimeEvent;
    cMessage *wakeUpServerEvent;


    bool serverIsAvailable; // true if Q2 @ this location
    bool serverIsIdle; // true if server associated is not processing any jobs
    bool isQ2LastLocation; // true if Q2 was here before and now must switch


public:
    OppQueue();
    virtual ~OppQueue();
    int length();

    // TODO: add getters/setters

protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void finish() override;

    // hook functions to (re)define behavior
    virtual void arrival(Job *job);
    virtual simtime_t startService(Job *job);
    virtual void endService(Job *job, int gateIndex);
};

}; //namespace
#endif /* OPPQUEUE_H_ */
