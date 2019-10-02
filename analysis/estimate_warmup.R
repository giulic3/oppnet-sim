#!/usr/bin/env Rscript
library(ggplot2)
source("compute.R")
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

# Compute throughput
throughput <- ThroughputOverTime(lifeTime)
throughput_df <- data.frame("simtime" = lifeTime[,1], "avgthroughput" = throughput, stringsAsFactors = FALSE)

x_axis_limit = 3000
y_axis_limit = 10 

cat("Save data plots...\n")
ggplot(q1Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4, color="black") + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Length of Q1 over time")
ggsave("q1Length_plot.pdf")
ggplot(q2Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Length of Q2 over time")
ggsave("q2Length_plot.pdf")
ggplot(q3Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Length of Q3 over time")
ggsave("q3Length_plot.pdf")
ggplot(lifeTime_df, aes(x=simtime,y=lifetime)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 60)) + ggtitle("Lifetimes of incoming jobs")
ggsave("lifeTime_plot.pdf")

cat("Save throughput plot...\n")
ggplot(throughput_df, aes(x=simtime, y=avgthroughput)) + geom_line(size=0.4) + coord_cartesian(xlim = c(10, 3000), ylim = c(0, 1)) + ggtitle("Network throughput")
ggsave("throughput_mean_plot.pdf") 