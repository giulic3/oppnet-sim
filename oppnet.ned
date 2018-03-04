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

import org.omnetpp.queueing.Queue;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;

simple OppQueue extends Queue
{
    @class(OppQueue);
}


network OppNet
{
    @display("bgb=487,190");
    submodules:
        source: Source {

            @display("p=43,82");
        }

        Q1: OppQueue {

            @display("p=140,82");
        }

        Q2: OppQueue {

            @display("p=230,82");
        }

        Q3: Queue {

            @display("p=312,82");
        }

        sink: Sink {

            @display("p=421,82");
        }

    connections:
        source.out --> Q1.in++;
        Q1.out --> Q2.in++;
        Q2.out --> Q3.in++;
        Q3.out --> sink.in++;
}

