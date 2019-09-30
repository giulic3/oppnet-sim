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
  for (i in 1:length){
    last_ts <- lifeTime_array[i,1]
    throughput_array[i] <- i / (last_ts - first_ts)
  }
  return(throughput_array)
}

# x = vector containing the values
# k = number of the first observations to ignore (to exclude warm up period)
# numBatches = number of batches
# numObsBatch = number of observations per batch
# d = precision, number of digits after comma
BatchMeans <- function(x, k, numBatches, numObsBatch, d) {
  
  stopifnot(k + numBatches * numObsBatch <= length(x))
  # Initialize an array filled with 0s
  means <- rep(0, numBatches)
  
  # Repeat numBatches times
  for (i in 1:numBatches) {
    a <- (i-1) * numObsBatch + 1 + k
    b <- i * numObsBatch + k
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
