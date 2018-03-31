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
// TODO documentation
//

package org.omnetpp.queueing;


network OppNet2
{
    @display("bgb=622.74005,313.31003");
    submodules:
        source: Source {
            @display("p=103.79001,211.46");
        }

        Q1: PassiveOppQueue {
            @display("p=200.79001,211.46");

            serverIsAvailable = true;
            isQ2LastLocation = true;
        }

        Q2: PassiveOppQueue {
            @display("p=291.00003,211.46");
        }

        S1: Server {

            @display("p=253.17001,98.94");
        }
        Q3: Queue {
            @display("p=372.48,211.46");
        }

        sink: Sink {
            @display("p=481.12003,211.46");
        }

    connections:
        source.out --> Q1.in++;
        Q1.out++ --> S1.in++;
        Q1.out++ --> Q2.in++;
        Q2.out++ --> S1.in++;
        Q2.out++ --> Q3.in++;
        Q3.out --> sink.in++;
        
}
