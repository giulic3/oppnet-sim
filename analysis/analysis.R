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
        cat('iteration: ', i,'\n')
        a <- (i-1) * numObs + 1 + k
        b <- i * numObs + k
        cat('a =', a, 'b =',b,'\n')
        # Extract the vector portion that contains the batch
        batch <- x[a:b]
        # Compute mean removing missing values
        means[i] <- mean(batch, na.rm=TRUE)
        cat('mean =', means[i],'\n\n')
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

# MAIN
setwd("../results/")

# First create a header for the csv file where results will be saved
header <- c("config name", "q1 length: mean", "q1 length: var", "q1 length: int l", "q1 length: int r",
            "q2 length: mean", "q2 length: var", "q2 length: int l", "q2 length: int r",
            "q3 length: mean", "q3 length: var", "q3 length: int l", "q3 length: int r",
            "lifetime: mean", "lifetime: var ", "lifetime: int l", "lifetime: int r")
header <- as.matrix(t(header))
write.table(header, file="results.csv", append=TRUE, sep=",", col.names=FALSE, row.names=FALSE)

# 3 params for interArrivalTime
iterationVars <- c(2.5, 5, 10)
runs <- 10
for (it in 1:length(iterationVars)) {
  for (j in 0:runs+1) {
      q1Length <-read.csv(paste("General-",iterationVars[it],"-#",j,"_q1length.csv", sep=""), header=TRUE, sep=',')
      #cat(paste("General-",iterationVars[i],"-#",j,"_q1length.csv", sep=""), '\n')
      q2Length <-read.csv(paste("General-",iterationVars[it],"-#",j,"_q2length.csv", sep=""), header=TRUE, sep=',')
      q3Length <-read.csv(paste("General-",iterationVars[it],"-#",j,"_q3length.csv", sep=""), header=TRUE, sep=',')
      lifeTime <-read.csv(paste("General-",iterationVars[it],"-#",j,"_lifetime.csv", sep=""), header=TRUE, sep=',')
      
      len1 <- length(q1Length[,2])
      len2 <- length(q2Length[,2])
      len3 <- length(q3Length[,2])
      len4 <- length(lifeTime[,2])
      r <- matrix(data=0, nr=4, nc=4) # Results matrix
      
      # TODO Check if results are correct
      numDigits <- 4
      cat('BatchMeans q1Length: \n')
      r[1,] <- BatchMeans(q1Length[,2], k=len1/10, numBatches=8, numObs=len1/10, d=numDigits)
      cat('BatchMeans q2Length: \n')
      r[2,] <- BatchMeans(q2Length[,2], k=len2/10, numBatches=8, numObs=len2/10, d=numDigits)
      cat('BatchMeans q3Length: \n')
      r[3,] <- BatchMeans(q3Length[,2], k=len3/10, numBatches=8, numObs=len3/10, d=numDigits)
      cat('BatchMeans lifeTime: \n')
      r[4,] <- BatchMeans(lifeTime[,2], k=len4/10, numBatches=8, numObs=len4/10, d=numDigits)
      
      
      for (i in 1:4) {
          cat('results are:\nfinalMean = ', r[i,1],'\nvariance = ', r[i,2],
          '\nconfidenceInterval = (', r[i,3], ',', r[i,4], ')\n\n')
      }
      # TODO the problem is that Little's Law assumes stable queues so check if the queues are stable
      meanThroughput <- (r[1,1]+r[2,1]+r[3,1])/r[4,1]
      cat('mean throughput = ', meanThroughput, '\n')
      
      # Save results to .csv
      results_column <- c(paste("General-",iterationVars[it],"-#",j, sep=""))
      for (i in 1:4)
        results_column <- c(results_column, r[i,1], r[i,2], r[i,3], r[i,4])
      results_row <- as.matrix(t(results_column))
      cat(results_row, "\n")
      write.table(results_row, file="results.csv", append=TRUE, sep=",", col.names=FALSE, row.names=FALSE)
  }
}