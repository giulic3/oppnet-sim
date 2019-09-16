#!/usr/bin/env bash
# Execute from results directory
# Export queues length and jobs lifetime
find -iname '*.vec' | while read name; do
    out=$(echo $name | sed 's/\.vec//')
    echo name $name
    echo out $out
    scavetool export --filter "module("OppNet.sink") AND lifeTime:vector" -o ${out}_lifetime.csv -F CSV-S $name
    scavetool export --filter "module("OppNet.Q1") AND queueLength:vector" -o ${out}_q1length.csv -F CSV-S $name
    scavetool export --filter "module("OppNet.Q2") AND queueLength:vector" -o ${out}_q2length.csv -F CSV-S $name
    scavetool export --filter "module("OppNet.Q3") AND queueLength:vector" -o ${out}_q3length.csv -F CSV-S $name
done
