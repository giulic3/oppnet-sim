# first execute scavetool x *.sca -o oppnet-sim.csv or use omnet editor
# click on .anf, right click on measure, export data, csv for spreadsheets
# execute with
# Rscript analysis.R
# library("ggplot2")
setwd("../results/")

# TODO maybe it's better to pass to the r script the csv files as arguments
# reads omnetpp vectors into data frames
q1Length <- read.csv("./General-10-#0_q1length.csv", header=TRUE, sep=',')
q2Length <- read.csv("./General-10-#0_q2length.csv", header=TRUE, sep=',')
q3Length <- read.csv("./General-10-#0_q3length.csv", header=TRUE, sep=',')
# lifeTime of a job from source to sink
lifeTime <- read.csv("./General-10-#0_lifetime.csv", header=TRUE, sep=',')

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
    confidenceIntervalLeft <- round(finalMean - a, digits=d)
    confidenceIntervalRight <- round(finalMean + a, digits=d)
    # c() is a function that combines its arguments to form a vector
    return (c(finalMean, variance, confidenceIntervalLeft, confidenceIntervalRight))
}
len1 <- length(q1Length[,2])
len2 <- length(q2Length[,2])
len3 <- length(q3Length[,2])
len4 <- length(lifeTime[,2])
# results matrix
r <- matrix(data=0, nr=4, nc=4)
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

# TODO Improve charts style, see also ggplot2
# Plotting the three queue lengths
plot(q1Length[,1], q1Length[,2], type="b", main="Length of Q1 over time", xlab="simtime",
    ylab="queue length", xlim=c(0, 500), ylim=c(0, 100), pch=20, cex=1)
plot(q2Length[,1], q2Length[,2], type="b", main="Length of Q2 over time", xlab="simtime",
    ylab="queue length", xlim=c(0, 100), ylim=c(0, 2), pch=20, cex=1)
plot(q3Length[,1], q3Length[,2], type="b", main="Length of Q3 over time", xlab="simtime",
    ylab="queue length", xlim=c(0, 2000), ylim=c(0, 2), pch=20, cex=1)
# Plotting the jobs lifetime
plot(lifeTime[,1], lifeTime[,2], type="b", main="Jobs lifetime", xlab="simtime",
    ylab="lifetime", xlim=c(0, 1000), ylim=c(0, 1000), pch=20, cex=1)
# Plotting the throughput, how?
# TODO
