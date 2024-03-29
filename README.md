# oppnet-sim
OMNeT++ simulation of a tandem queueing model for opportunistic networks

## Getting Started

Follow this README.md to have a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

* [OMNeT++](https://omnetpp.org/) - Discrete Event Simulator
* [OMNeT++ - queueinglib](https://github.com/shigeya/omnetpp/tree/master/samples/queueinglib) - OMNeT++ Library for implementing queueing networks
* [R](https://www.r-project.org/) - Language and Environment for Statistical Computing


### Installing

To get and access the code:


```
git clone https://github.com/giulic3/oppnet-sim.git
cd oppnet-sim/

```

## Running the code

### Running the simulation


See runs that have been configured (cmdline env):
```
./oppnet-sim -u Cmdenv -x General -g
```
To run the simulation, from the executable directory:

```
./oppnet-sim -m -n <path-to-NED-files> -l <path-to-queueinglib-shared-library> <ini-file> -u Cmdenv -c General

```

Useful flags:
```
-m                  # merge standard error into standard output, i.e. report errors on
                      the standard output instead of the default standard error.
-n                  # when present, overrides the NEDPATH environment variable.
-l                  # load shared libraries
-u Cmdenv (Qtenv)   # select user interface/runtime environment
-c <config-name>    # select config
-r <run>            # select run

```

### Extract the statistics and perform analysis in R

Execute script to extract .csv files from .sca and .vec files produced by OMNeT++:
```
bash export_stats.sh
```
Execute inside .csv directory (csv/{2.5, 5, 10}), will plot response variables from .csv files provided and the averaged 
variables from multiple sim replications:
```
Rscript estimate_warmup.R <q1length.csv, q2length.csv, q3length.csv, lifetime.csv>
```
Sample simulation outputs (after removing warm up period) and save results to .csv:

```
Rscript analysis.R
```

For more details, see doc/ folder (in Italian only).
