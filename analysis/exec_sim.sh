#!/bin/bash

# Run simulation (preliminary or batch according to arg = [PreliminarySimulation | BatchSimulation])
./oppnet-sim -m -n .:../../Scaricati/omnetpp-5.4/samples/queueinglib -l ../../Scaricati/omnetpp-5.4/samples/queueinglib/queueinglib omnetpp.ini -c %1 -u Cmdenv

# Export stats
cd ../results/
source ./export_stats.sh

# Run R scripts
Rscript estimate_warmup.R
Rscript analysis.R
