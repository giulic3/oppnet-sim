#!/usr/bin/env Rscript
# This script produces plots for selected csv files and all the replications in a folder in order to estimate warm up period

library(ggplot2)
source("compute.R")

args = commandArgs(trailingOnly=TRUE)
# Test if the 4 arguments are provided
if (length(args) == 0) {
  stop("Four arguments must be supplied as .csv:
  - q1length
  - q2length
  - q3length
  - lifetime", call.=FALSE)
}

sim_length <- 10000
# Reads omnetpp vectors into data frames
q1Length <- read.csv(args[1], sep=',')
q2Length <- read.csv(args[2], sep=',')
q3Length <- read.csv(args[3], sep=',')
lifeTime <- read.csv(args[4], sep=',') # lifeTime of a job from source to sink

setwd("./csv/10") # TODO: Set dir according to param to study
# Read csv from directory and convert to array with simtime as first column
# Order is lifetime - q1length - q2length - q3length
csv_files <- list.files(pattern="*.csv")
csv_dfs <- list()
cat("Convert to simtime...\n")
for (i in 1:length(csv_files)) csv_dfs[[i]] = ConvertToSimtime(read.csv(csv_files[i], sep=','), sim_length)

cat("Compute mean averaged between replications...\n")
q1Length_avg <- matrix(0, nrow=sim_length, ncol=2)
q2Length_avg <- matrix(0, nrow=sim_length, ncol=2)
q3Length_avg <- matrix(0, nrow=sim_length, ncol=2)
lifeTime_avg <- matrix(0, nrow=sim_length, ncol=2)

lifeTimes <- list(csv_dfs[[1]], csv_dfs[[5]], csv_dfs[[9]])
q1Lengths <- list(csv_dfs[[2]], csv_dfs[[6]], csv_dfs[[10]])
q2Lengths <- list(csv_dfs[[3]], csv_dfs[[7]], csv_dfs[[11]])
q3Lengths <- list(csv_dfs[[4]], csv_dfs[[8]], csv_dfs[[12]])

measures_list <- list(q1Lengths, q2Lengths, q3Lengths, lifeTimes)
avg_measures_list <- list(q1Length_avg, q2Length_avg, q3Length_avg, lifeTime_avg)

for (i in 1:sim_length) {
  for (m in 1:length(measures_list)) {
    current_measure <- measures_list[[m]]
    current_avg_measure <- avg_measures_list[[m]]
    
    avg_measures_list[[m]][i,1] <- i
    avg_measures_list[[m]][i,2] <- mean(c(current_measure[[1]][i,2], current_measure[[2]][i,2], current_measure[[3]][i,2]), na.rm=TRUE)
  }
}
cat("Compute throughput...\n")
throughput <- ThroughputOverTime(lifeTime)

# Prepare dframes to use with ggplot2
q1Length_df <- data.frame("simtime" = q1Length[,1], "length" = q1Length[,2], stringsAsFactors = FALSE)
q2Length_df <- data.frame("simtime" = q2Length[,1], "length" = q2Length[,2], stringsAsFactors = FALSE)
q3Length_df <- data.frame("simtime" = q3Length[,1], "length" = q3Length[,2], stringsAsFactors = FALSE)
lifeTime_df <- data.frame("simtime" = lifeTime[,1], "lifetime" = lifeTime[,2], stringsAsFactors = FALSE)

avg_dfs <- list()
for (i in 1:length(avg_measures_list))
  avg_dfs[[i]] <- data.frame("simtime" = avg_measures_list[[i]][,1], "avgvalue" = avg_measures_list[[i]][,2], stringsAsFactors = FALSE)

throughput_df <- data.frame("simtime" = lifeTime[,1], "avgthroughput" = throughput, stringsAsFactors = FALSE)

# Create and save plots
x_axis_limit = 3000
y_axis_limit = 10 

cat("Save plots...\n")
ggplot(q1Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4, color="black") + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Length of Q1 over time")
#ggsave("q1Length_plot.pdf")
ggplot(q2Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Length of Q2 over time")
ggplot(q3Length_df, aes(x=simtime,y=length)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Length of Q3 over time")
ggplot(lifeTime_df, aes(x=simtime,y=lifetime)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 60)) + ggtitle("Lifetimes of incoming jobs")

ggplot(avg_dfs[[1]], aes(x=simtime,y=avgvalue)) + geom_line(size=0.4, color="black") + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 4)) + ggtitle("Avg length of Q1 over time")
ggplot(avg_dfs[[2]], aes(x=simtime,y=avgvalue)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 4)) + ggtitle("Avg length of Q2 over time")
ggplot(avg_dfs[[3]], aes(x=simtime,y=avgvalue)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 4)) + ggtitle("Avg length of Q3 over time")
ggplot(avg_dfs[[4]], aes(x=simtime,y=avgvalue)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 30)) + ggtitle("Avg lifetimes of incoming jobs")

ggplot(throughput_df, aes(x=simtime, y=avgthroughput)) + geom_line(size=0.4) + coord_cartesian(xlim = c(10, x_axis_limit), ylim = c(0, 1)) + ggtitle("Network throughput")