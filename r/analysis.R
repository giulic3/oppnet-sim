# first execute scavetool x *.sca -o oppnet-sim.csv or use omnet editor
# click on .anf, right click on measure, export data, csv for spreadsheets

# how to run a R script with output and input redirection
# R CMD BATCH [options] my_script.R [outfile]
# or with output to the terminal
# Rscript analysis.R

# TODO
# change to the R script directory
setwd("/home/giulia/git/oppnet-sim/")
# alternatively, set the absolute path when reading csv

# reads omnetpp vectors into data frames
q1Length <- read.csv("./results/q1length.csv", header=TRUE, sep=',')
q2Length <- read.csv("./results/q2length.csv", header=TRUE, sep=',')
q3Length <- read.csv("./results/q3length.csv", header=TRUE, sep=',')
# lifeTime of a job from source to sink
lifeTime <- read.csv("./results/lifetime.csv", header=TRUE, sep=',')

# x = vector containing the values
# k = number of the first observations to ignore (to exclude warm up period)
# numBatches = number of batches
# numObs = number of observations per batch
# dprecision = number of digits after comma
BatchMeans <- function(x, k, numBatches, numObs, d) {

    stopifnot(k + numBatches * numObs <= length(x))
    # initialize an array filled with 0s
    means <- rep(0, numBatches)

    # repeat numBatches times
    for (i in 1:numBatches) {
        cat('iteration: ', i,'\n')
        a <- (i-1) * numObs + 1 + k
        b <- i * numObs + k
        cat('a =', a, 'b =',b,'\n')
        # extract the vector portion that contains the batch
        batch <- x[a:b]
        # compute mean removing missing values
        means[i] <- mean(batch, na.rm=TRUE)
        cat('mean =', means[i],'\n\n')
    }

    finalMean <- round(mean(means), digits=d)
    # variance is computed using N-1 at the bottom of the fraction
    variance <- round(var(means), digits=d)
    n <- length(means)
    # TODO  check if it's right
    # confidence level 95%, qt quantile function, df degrees of freedom
    a <- round(qt(0.975, df = n - 1) * sqrt(variance/n), digits=d)
    # quantile(x, probs=0.25,...)
    confidenceIntervalLeft <- round(finalMean - a, digits=d)
    confidenceIntervalRight <- round(finalMean + a, digits=d)
    # c() is a function that combines its arguments to form a vector
    return (c(finalMean, variance, confidenceIntervalLeft, confidenceIntervalRight))
}
length(q1Length[,2])
length(q2Length[,2])
length(q3Length[,2])
length(lifeTime[,2])
# results matrix
r <- matrix(data=0, nr=4, nc=4)
numDigits <- 4
# TODO how to choose parameters
cat('BatchMeans q1Length: \n')
r[1,] <- BatchMeans(q1Length[,2], k=1000, numBatches=13, numObs=700, d=numDigits)
cat('BatchMeans q2Length: \n')
r[2,] <- BatchMeans(q2Length[,2], k=1000, numBatches=11, numObs=600, d=numDigits)
cat('BatchMeans q3Length: \n')
r[3,] <- BatchMeans(q3Length[,2], k=100, numBatches=5, numObs=150, d=numDigits)
cat('BatchMeans lifeTime: \n')
r[4,] <- BatchMeans(lifeTime[,2], k=500, numBatches=9, numObs=300, d=numDigits)

#avgSojournTime =
#avgUsers =

for (i in 1:4) {
    cat('results are:\nfinalMean = ', r[i,1],'\nvariance = ', r[i,2],
    '\nconfidenceInterval = (', r[i,3], ',', r[i,4], ')\n\n')
}
