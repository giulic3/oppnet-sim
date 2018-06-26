# batch analysis
# first execute scavetool x *.sca -o oppnet-sim.csv or use omnet editor
# TODO check which logs must be analyzed and automate the creation of .csv or use .json
# perché ad ogni export ho sempre lo stesso numero di record?

# change to the R script directory
setwd("/home/giulia/git/oppnet-sim/")

# reads into a frame data?
simData <- read.csv("./results/oppnet-sim.csv", header=TRUE, sep=',')
dim(simData)

# RemoveWarmUpPeriod or RemoveInitialTransient... to remove the first m data

# x = vector containing the values
# numBatches = number of batches
# numObs = number of observations per batch
BatchMeans <- function(x, numBatches, numObs) {

  means <- rep(0, numBatches)
  # observations to ignore
  k <- 1000 #use RemoveWarmpUpPeriod

  # repeat numBatches times
  for (i in 1:numBatches) {

    #for (j in numObs)
    a <- i * numObs + 1 # not sure
    b <- i * (numObs + 1) + 1 # not sure
     # extract the vector portion that contains the batch
    batch <- x[a:b]
    means[i] <- mean(batch)
    print(means[i])

  #}
  }
  finalMean <- mean(means)
  standardDeviation <- sd(means)
  # anche varianza?
  #confidenceIntervalLeft <-
  #confidenceIntervalRight <-

  

  return c(finalMean, standardDeviation, confidenceIntervalLeft, confidenceIntervalRight)

}

results = BatchMeans(X, N, n)
a <- r[1]
b <- r[2]
c <- r[3]
d <- r[4]

# batchMeans(totalServiceTimes,0,0)
# batchMeans(numberOfJobs,0,0)
# little's law...
# ?
