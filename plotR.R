plotData <-function(){
mydata <- read.csv("/home/atul/UMBC/thesis/data/notes/Perception+confidence+ask/density//30/log_1000_d_30.csv", header=TRUE)
#mydata <- read.csv("/home/atul/Dropbox/thesis_car/ActiveAndQLearn/analysis/height 250/corrected//1000//logAnalysis_1000_corrected.csv", header=TRUE)

#calculate max and mins
max_y <- 0
min_y <- 0
for (i in 1:length(mydata)){
        min <- min(mydata[[i]],na.rm = TRUE)
        if(min<min_y)
        {
                min_y = min
        }
        
        max <- max(mydata[[i]],na.rm = TRUE)
        if(max > max_y)
        {
                max_y = max
        }
}
plot(0,0,xlim = c(1,length(mydata[[1]])),ylim = c(min_y,max_y),xlab = "Episodes", ylab = "Number of Misses", main = "Misses vs Episode Number", )
cl <- rainbow(length(mydata))

for (i in 1:length(mydata)){
        lines(1:length(mydata[[1]]),mydata[[i]],col = cl[i],type = "S",pch = rep(2, length.out=10))
        
}
legend("topright", legend=names(mydata), lwd=1,
       col=cl,fill = 1:6, ncol = 2,
       cex = 0.75)
}

plotData();