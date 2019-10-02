#!/usr/bin/env Rscript

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
  measures <- cbind(means, variances)
  return(measures)
}
# Function that given the array of jobs lifetimes, return array of throughput over time for plotting
ThroughputOverTime <- function(lifeTime_array){
  first_ts <- lifeTime_array[1,1]
  length <- length(lifeTime_array[,2])
  throughput_array <- array(data = 0, dim = length)
  for (i in 2:length){
    last_ts <- lifeTime_array[i,1]
    throughput_array[i] <- i / (last_ts - first_ts)
  }
  return(throughput_array)
}

# Function that translates a 2D array of the kind (value, simtime when event occurred) to 2D array
# of kind (value, simtime) where simtime goes from 0 to sim-time-limit 
ConvertToSimtime <- function(two_dim_array) {
  time_limit <- 3200 # sim-time-limit of the PreliminarySimulation
  length <- length(two_dim_array[,1])
  new_array <- array(0, c(time_limit, 2))
  previous_index = 1
  value <- 0
  for (i in 1:(length-1)) {
    event_time <- trunc(two_dim_array[i,1])
    value <- trunc(two_dim_array[i,2])
    for (j in previous_index:event_time-1) {
      new_array[j,1] <- j
      new_array[j,2] <- value
    }
    previous_index <- event_time
  }
  cat("previous index", previous_index, "\n")
  cat("time_limit", time_limit, "\n")
  cat("value", value, "\n")
  for (i in previous_index:time_limit) {
    new_array[i,1] <- i
    new_array[i,2] <- value
  }
  return(new_array)
}

# Assumption: k simtime observations were already dropped in omnetpp
# x = vector containing the values
# numBatches = number of batches
# numObsBatch = number of observations per batch
# d = precision, number of digits after comma
BatchMeans <- function(x, numBatches, numObsBatch, d) {
  # print(numBatches * numObsBatch)
  # print(length(x))
  stopifnot(numBatches * numObsBatch <= length(x))
  # Initialize an array filled with 0s
  means <- rep(0, numBatches)
  
  # Repeat numBatches times
  for (i in 1:numBatches) {
    a <- (i-1) * numObsBatch + 1
    b <- i * numObsBatch
    # Extract the vector portion that contains the batch
    batch <- x[a:b]
    # Compute mean removing missing values
    means[i] <- mean(batch, na.rm=TRUE)
  }
  
  finalMean <- round(mean(means), digits=d)
  # Variance is computed using N-1 at the bottom of the fraction
  variance <- round(var(means), digits=d)
  n <- length(means)
  # Confidence level 95%, qt quantile function, df degrees of freedom
  #a <- round(qt(0.975, df = n - 1) * sqrt(variance/n), digits=d)
  a <- round(quantile(means, probs=0.75, na.rm = TRUE), digits=d)
  confidenceIntervalLeft <- round(finalMean - a, digits=d)
  confidenceIntervalRight <- round(finalMean + a, digits=d)
  # Result is a concatenation of these vectors
  return (c(finalMean, variance, confidenceIntervalLeft, confidenceIntervalRight))
}
