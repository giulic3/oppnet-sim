setwd("../results/")

args = commandArgs(trailingOnly=TRUE)
# Test if there is at least one argument: if not, return an error
# TODO But I have to average all the runs
if (length(args) == 0) {
  stop("Four arguments must be supplied as .csv:
  - q1length
  - q2length
  - q3length
  - lifetime", call.=FALSE)
}

# list.files(path = "../results/", pattern="%.csv")
# Reads omnetpp vectors into data frames
# q1Length <- read.csv(args[1], header=TRUE, sep=',')
#q2Length <- read.csv(args[2], header=TRUE, sep=',')
#q3Length <- read.csv(args[3], header=TRUE, sep=',')
#lifeTime <- read.csv(args[4], header=TRUE, sep=',') # lifeTime of a job from source to sink


# if showPlot then... gli passi solo 4 params e fa i plot
# TODO Improve charts style, see also ggplot2
# TODO I would like to plot the mean directly on the graphs for the queuelengths
# Plotting the three queues lengths
plot(q1Length[,1], q1Length[,2], type="b", main="Length of Q1 over time", xlab="simtime",
     ylab="queue length", xlim=c(0, 800), ylim=c(0, 5), pch=20, cex=1)
plot(q2Length[,1], q2Length[,2], type="b", main="Length of Q2 over time", xlab="simtime",
     ylab="queue length", xlim=c(0, 100), ylim=c(0, 2), pch=20, cex=1)
plot(q3Length[,1], q3Length[,2], type="b", main="Length of Q3 over time", xlab="simtime",
     ylab="queue length", xlim=c(0, 2000), ylim=c(0, 2), pch=20, cex=1)
# Plotting the jobs lifetime
plot(lifeTime[,1], lifeTime[,2], type="b", main="Jobs lifetime", xlab="simtime",
     ylab="lifetime", xlim=c(0, 1000), ylim=c(0, 1000), pch=20, cex=1)
# Plotting the throughput, how?
# TODO

