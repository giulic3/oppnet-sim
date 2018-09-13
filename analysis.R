# first execute scavetool x *.sca -o oppnet-sim.csv or use omnet editor
# click on .anf, right click on measure, export data, csv for spreadsheets

# how to run a R script with output and input redirection
# R CMD BATCH [options] my_script.R [outfile]
# or with output to the terminal
# Rscript analysis.R

# change to the R script directory
setwd("/home/giulia/git/oppnet-sim/")
# alternatively, set the absolute path when reading csv

# reads omnetpp vectors into data frames
q1Length = read.csv("./results/q1length.csv", header=TRUE, sep=',')
q2Length = read.csv("./results/q2length.csv", header=TRUE, sep=',')
q3Length = read.csv("./results/q3length.csv", header=TRUE, sep=',')
#totalServiceTime = read.csv("./results/totalservicetime.csv", header=TRUE, sep=',')

# TODO see if exists a built in function for array traslation
# x = vector containing the values
# k = number of the first observations to ignore
RemoveWarmUpPeriod <- function(x, k) { # or RemoveInitialTransient
    dimY <- length(x) - k
    y <- rep(0, dimY)
    for (i in 1:dimY)
        y[i] <- x[i + k]

    return (y)
}

# x = vector containing the values
# numBatches = number of batches
# numObs = number of observations per batch
BatchMeans <- function(x, numBatches, numObs) {

    if (numBatches * numObs > length(x))
        cat('error!')
        # TODO should exit
    # initialize an array filled with 0s
    means <- rep(0, numBatches)

    # repeat numBatches times
    for (i in 1:numBatches) {
        cat('iteration: ', i,'\n')
        #for (j in numObs)
        a <- (i-1) * numObs + 1 # not sure
        b <- i * numObs # not sure
        cat('a =', a, 'b =',b,'\n')

        # extract the vector portion that contains the batch
        batch <- x[a:b]
        # compute mean removing missing values
        means[i] <- mean(batch, na.rm=TRUE)
        cat('mean =', means[i],'\n\n')
    }

    finalMean <- round(mean(means), digits=2)
    # variance is computed using N-1 at the bottom of the fraction
    variance <- round(var(means), digits=2)
    n <- length(means)
    # TODO  check if it's right
    # confidence level 95%, qt quantile function, df degrees of freedom
    a <- round(qt(0.975, df = n -1) * sqrt(variance/n), digits=2)
    # quantile(x, probs=0.25,...)
    confidenceIntervalLeft <- round(finalMean - a, digits=2)
    confidenceIntervalRight <- round(finalMean + a, digits=2)
    # c() is a function that combines its arguments to form a vector
    return (c(finalMean, variance, confidenceIntervalLeft, confidenceIntervalRight))
}



q1LengthVector = q1Length[,2]
q2LengthVector = q2Length[,2]
#q3LengthVector = q3Length[,2]
# TODO need to sum total service time and total queueing time?
#totalServiceTimeVector = totalServiceTime[,2]
#totalQueueingTimeVector

newX <- RemoveWarmUpPeriod(q1LengthVector, 1000)
results <- BatchMeans(newX, 13, 700)
#avgSojournTime =
#avgUsers =

cat('results are:\nfinalMean = ', results[1],'\nvariance = ', results[2],
'\nconfidenceInterval = (', results[3], ',', results[4], ')\n')
