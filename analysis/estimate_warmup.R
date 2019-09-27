#!/usr/bin/env Rscript
library(ggplot2)

# Function that given a vector of values and timesteps, return array of cumulative means and variances
# 'array' must have two dimensions
CumulativeMeasures <- function(array) {
  count <- length(array[,1])
  means <- array(data = 0, dim = count)
  variances <- array(data = 0, dim = count)
  for (i in 1:count) {
    means[i] <- mean(array[1,2]:array[i,2], na.rm=TRUE)
    variances[i] <- var(array[1,2]:array[i,2], na.rm=TRUE) 
  }
  
  return(c(means, variances))
}

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

cat("Compute cumulative means...\n")
start.time <- Sys.time()

q1Length_mean_df <- data.frame("simtime" = q1Length[,1], "avglength" = CumulativeMeasures(q1Length), stringsAsFactors = FALSE)
q2Length_mean_df <- data.frame("simtime" = q2Length[,1], "avglength" = CumulativeMeasures(q2Length), stringsAsFactors = FALSE)
q3Length_mean_df <- data.frame("simtime" = q3Length[,1], "avglength" = CumulativeMeasures(q3Length), stringsAsFactors = FALSE)
lifeTime_mean_df <- data.frame("simtime" = lifeTime[,1], "avglifetime" = CumulativeMeasures(lifeTime), stringsAsFactors = FALSE)

q1Length_var_df <- data.frame("simtime" = q1Length[,1], "varlength" = CumulativeMeasures(q1Length), stringsAsFactors = FALSE)
q2Length_var_df <- data.frame("simtime" = q2Length[,1], "varlength" = CumulativeMeasures(q2Length), stringsAsFactors = FALSE)
q3Length_var_df <- data.frame("simtime" = q3Length[,1], "varlength" = CumulativeMeasures(q3Length), stringsAsFactors = FALSE)
lifeTime_var_df <- data.frame("simtime" = lifeTime[,1], "varlifetime" = CumulativeMeasures(lifeTime), stringsAsFactors = FALSE)

end.time <- Sys.time()
time.taken <- end.time - start.time
time.taken

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

cat("Save mean plots...\n")
ggplot(q1Length_mean_df, aes(x=simtime,y=avglength)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Average length of Q1")
ggsave("q1Length_mean_plot.pdf")
ggplot(q2Length_mean_df, aes(x=simtime,y=avglength)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Average length of Q2")
ggsave("q2Length_mean_plot.pdf")
ggplot(q3Length_mean_df, aes(x=simtime,y=avglength)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Average length of Q3")
ggsave("q3Length_mean_plot.pdf")
ggplot(lifeTime_mean_df, aes(x=simtime,y=avglifetime)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Average job lifetime")
ggsave("lifeTime_mean_plot.pdf")

cat("Save variance plots...\n")
ggplot(q1Length_var_df, aes(x=simtime,y=varlength)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Variance on length of Q1")
ggsave("q1Length_var_plot.pdf") 
ggplot(q2Length_var_df, aes(x=simtime,y=varlength)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Variance on length of Q2")
ggsave("q2Length_var_plot.pdf")
ggplot(q3Length_var_df, aes(x=simtime,y=varlength)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Variance on length of Q3")
ggsave("q3Length_var_plot.pdf")
ggplot(lifeTime_var_df, aes(x=simtime,y=varlifetime)) + geom_line(size=0.4) + coord_cartesian(xlim = c(0, x_axis_limit), ylim = c(0, 10)) + ggtitle("Variance on job lifetime")
ggsave("lifeTime_var_plot.pdf")
