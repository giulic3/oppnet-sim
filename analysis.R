# batch analysis
# first execute scavetool x *.sca -o oppnet-sim.csv or use omnet editor
# TODO check which logs must be analyzed and automate the creation of .csv or use .json

# change to the R script directory
setwd("/home/giulia/git/oppnet-sim/")

simdata <- read.csv("./results/oppnet-sim.csv", header=TRUE, sep=',')
dim(simdata)

batch <- function(x, nb, nobs) {
  v <- rep(na,nb)
  qq <- (x[1:nobs])
  v[1] <- mean(qq)
  
  print(v[1])
  
  for (i in nb) {
    a <- i*nobs+(i-1)
    qq <- x[a:b]
    v[i] <- mean(qq)
    print(v[i])
    
  }
  
  a1 <- mean(v)
  b1 <- sd(v)
  # b2
  # c1 <- median
  # d1 <- quantile(v, probs=0.25, nar.rm=TRUE)
  # E1 <- quantile(v, probs=0.75, nar.rm=TRUE)
  
} 

batch(rstat,40,45)