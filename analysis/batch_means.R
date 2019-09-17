#!/usr/bin/env Rscript

# x = vector containing the values
# k = number of the first observations to ignore (to exclude warm up period)
# numBatches = number of batches
# numObs = number of observations per batch
# d = precision, number of digits after comma
BatchMeans <- function(x, k, numBatches, numObs, d) {
  
  stopifnot(k + numBatches * numObs <= length(x))
  # Initialize an array filled with 0s
  means <- rep(0, numBatches)
  
  # Repeat numBatches times
  for (i in 1:numBatches) {
    #cat('iteration: ', i,'\n')
    a <- (i-1) * numObs + 1 + k
    b <- i * numObs + k
    #cat('a =', a, 'b =',b,'\n')
    # Extract the vector portion that contains the batch
    batch <- x[a:b]
    # Compute mean removing missing values
    means[i] <- mean(batch, na.rm=TRUE)
    #cat('mean =', means[i],'\n\n')
  }
  
  finalMean <- round(mean(means), digits=d)
  # Variance is computed using N-1 at the bottom of the fraction
  variance <- round(var(means), digits=d)
  n <- length(means)
  # TODO  check if it's right
  # Confidence level 95%, qt quantile function, df degrees of freedom
  a <- round(qt(0.975, df = n - 1) * sqrt(variance/n), digits=d)
  confidenceIntervalLeft <- round(finalMean - a, digits=d)
  confidenceIntervalRight <- round(finalMean + a, digits=d)
  # Result is a concatenation of these vectors
  return (c(finalMean, variance, confidenceIntervalLeft, confidenceIntervalRight))
}
