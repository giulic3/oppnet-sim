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

network OppNet
{
    @display("bgb=539.32,277.42");
    submodules:
        source: Source {
            @display("p=43,82");
        }

        Q1: PassiveQueue {
            @display("p=140,82");
        }

        Q2: PassiveQueue {
            @display("p=246.38002,81.48");
        }

        S: OppServer {
            @display("p=246.38002,197.88");
        }

        Q3: Queue {
            @display("p=355.99002,81.48");
        }

        sink: Sink {
            @display("p=482.09003,81.48");
        }

    connections:
        source.out --> Q1.in[0];
        Q1.out[0] --> S.in[0];
        S.out[0] --> Q2.in[0]; // first S1 out goes to Q2
        Q2.out[0] --> S.in[1];
        S.out[1] --> Q3.in[0]; // second S2 out goes to Q3
        Q3.out --> sink.in++;
}

