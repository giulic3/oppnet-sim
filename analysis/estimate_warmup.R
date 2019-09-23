#!/usr/bin/env Rscript
library(ggplot2)

#setwd("../results/")

args = commandArgs(trailingOnly=TRUE)
# Test if the 4 arguments are provided
if (length(args) == 0) {
  stop("Four arguments must be supplied as .csv:
  - q1length
  - q2length
  - q3length
  - lifetime", call.=FALSE)
}

# Reads omnetpp vectors into data frames
q1Length <- read.csv(args[1], sep=',')

q2Length <- read.csv(args[2], sep=',')
q3Length <- read.csv(args[3], sep=',')
lifeTime <- read.csv(args[4], sep=',') # lifeTime of a job from source to sink

# Prepare dfs
q1Length_df <- data.frame("simtime" = q1Length[,1], "length" = q1Length[,2], stringsAsFactors = FALSE)
q2Length_df <- data.frame("simtime" = q2Length[,1], "length" = q2Length[,2], stringsAsFactors = FALSE)
q3Length_df <- data.frame("simtime" = q3Length[,1], "length" = q3Length[,2], stringsAsFactors = FALSE)
lifeTime_df <- data.frame("simtime" = lifeTime[,1], "lifetime" = lifeTime[,2], stringsAsFactors = FALSE)

# Plots
ggplot(q1Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4, color="black") + coord_cartesian(xlim = c(0, 500), ylim = c(0, 10))
ggsave("q1Length_plot.pdf")

ggplot(q2Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, 10000), ylim = c(0, 10))
ggsave("q2Length_plot.pdf")

ggplot(q3Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, 500), ylim = c(0, 10))
ggsave("q3Length_plot.pdf")

ggplot(lifeTime_df, aes(x=simtime,y=lifetime)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, 10000), ylim = c(0, 60))
ggsave("lifeTime_plot.pdf")

