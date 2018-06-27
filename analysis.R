# batch analysis
# first execute scavetool x *.sca -o oppnet-sim.csv or use omnet editor
# TODO check which logs must be analyzed and automate the creation of .csv or use .json
# perché ad ogni export ho sempre lo stesso numero di record?

# how to run a R script
# on Linux with output and input redirection
# R CMD BATCH [options] my_script.R [outfile]
# with output to the terminal
# Rscript analysis.R

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
    a <- (i-1) * numObs + 1 # not sure
    b <- i * numObs # not sure
    cat('a =', a, 'b =',b,'\n')

     # extract the vector portion that contains the batch
    batch <- x[a:b]
    # compute mean removing missing values
    means[i] <- mean(batch, na.rm=TRUE)
    cat('mean =',means[i],'\n')

  #}
  }
  finalMean <- round(mean(means), digits=2)
  variance <- round(var(means), digits=2)
  n <- length(means)
  # confidence level 95%, qt quantile function, df degrees of freedom
  a <- round(qt(0.975, df = n -1) * sqrt(variance/n), digits=2)
  confidenceIntervalLeft <- round(finalMean - a, digits=2)
  confidenceIntervalRight <- round(finalMean + a, digits=2)

  return (c(finalMean, variance, confidenceIntervalLeft, confidenceIntervalRight))

}

# testing (20 elements)
X <- c(1,2,3,4,5,6,7,8,9,10,11,12,13,14,6,5,55,3,4,1)
N <- 2
n <- 5

results = BatchMeans(X, N, n)
print(results)

a <- results[1]
b <- results[2]
c <- results[3]
d <- results[4]

# only for testing purposes
times <- c(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20)

# creating a graph
#attach(simPlot)
# output goes to pdf
plot(X, times)
title("Means plotted")

# batchMeans(totalServiceTimes,0,0)
# batchMeans(numberOfJobs,0,0)
# little's law...
# ?
