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

package org.omnetpp.queueing;

//
// Queue server. It serves multiple input queues (PassiveQueue), using an opportunistic
// algorithm. Inputs must be connected to Passive Queues (PassiveQueue)
//
simple OppServer
{
    parameters:
        @group(Queueing);
        @display("i=block/server");
        @signal[busy](type="bool");
        @statistic[busy](title="server busy state";record=vector?,timeavg;interpolationmode=sample-hold);
		
		// not really needed
		string fetchingAlgorithm @enum("priority","random","roundRobin","longestQueue") = default("priority");
		

        volatile double serviceTime @unit(s);
        volatile double switchOverTime @unit(s);
        volatile double Q1visitTime @unit(s);
        volatile double Q2visitTime @unit(s);
        bool isServingQ1 = default(true);
        bool serverIsAvailable = default(true);
        
        
    gates:
        input in[2];
        output out[2];
}