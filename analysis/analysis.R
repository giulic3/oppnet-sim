#!/usr/bin/env Rscript
source("compute.R")

# MAIN
setwd("../results/")

seeds <- c("11") # fill if you specified seeds in the .ini
# 3 params for interArrivalTime
iterationVars <- c(5, 7.5, 10)
runs <- 1 # this changes

n_rows <- length(seeds)*length(iterationVars)*runs
n_columns <- 21
# results <- array(0, c(n_rows,n_columns))

# First create a header for the csv file where results will be saved
header <- c("config name", "q1 length: mean", "q1 length: var", "q1 length: int l", "q1 length: int r",
            "q2 length: mean", "q2 length: var", "q2 length: int l", "q2 length: int r",
            "q3 length: mean", "q3 length: var", "q3 length: int l", "q3 length: int r",
            "lifetime: mean", "lifetime: var ", "lifetime: int l", "lifetime: int r",
            "throughput: mean", "throughput: var", "throughput: int l", "thoughput: int r")

results <- rbind(header)

for (s in 1:length(seeds)) {
  for (it in 1:length(iterationVars)) {
    for (j in 0:(runs-1)) {
        q1Length <-read.csv(paste("BatchSimulation-",seeds[s], ",", iterationVars[it],"-#",j,"_q1length.csv", sep=""), header=TRUE, sep=',')
        #cat(paste("BatchSimulation-",iterationVars[i],"-#",j,"_q1length.csv", sep=""), '\n')
        q2Length <-read.csv(paste("BatchSimulation-",seeds[s], ",", iterationVars[it],"-#",j,"_q2length.csv", sep=""), header=TRUE, sep=',')
        q3Length <-read.csv(paste("BatchSimulation-",seeds[s], ",", iterationVars[it],"-#",j,"_q3length.csv", sep=""), header=TRUE, sep=',')
        lifeTime <-read.csv(paste("BatchSimulation-",seeds[s], ",", iterationVars[it],"-#",j,"_lifetime.csv", sep=""), header=TRUE, sep=',')
        
        r <- matrix(data=0, nr=5, nc=4) # Results matrix
        numDigits <- 5
        numBatches <- 10 # 30,40,10
        numObsBatch <- 200 # 40,60,200
        
        # cat('BatchMeans q1Length: \n')
        r[1,] <- BatchMeans(q1Length[,2], numBatches, numObsBatch, d=numDigits)
        # cat('BatchMeans q2Length: \n')
        r[2,] <- BatchMeans(q2Length[,2], numBatches, numObsBatch, d=numDigits)
        # cat('BatchMeans q3Length: \n')
        r[3,] <- BatchMeans(q3Length[,2], numBatches, numObsBatch, d=numDigits)
        # cat('BatchMeans lifeTime: \n')
        r[4,] <- BatchMeans(lifeTime[,2], numBatches, numObsBatch, d=numDigits)
        # cat('BatchMeans throughput: \n')
        throughput <- ThroughputOverTime(lifeTime)
        r[5,] <- BatchMeans(throughput, numBatches, numObsBatch, d=numDigits)
        # meanThroughput <- round(len4 / (lifeTime[len4,1] - lifeTime[1,1]), 2)

        results_column <- c(paste("BatchSimulation-",seeds[s], ",", "-",iterationVars[it],"-#",j, sep=""))
        for (i in 1:5)
          results_column <- c(results_column, r[i,1], r[i,2], r[i,3], r[i,4])
        # results_column <- c(results_column, meanThroughput)
        results <- rbind(results, results_column)
    }
  }
}
print(results)
# Visualization with .csv is not to be trusted
write.table(results, file="../analysis/results.csv", append=TRUE, sep="\t", col.names=FALSE, row.names=FALSE)
cat("\n", "### done ###", "\n")

