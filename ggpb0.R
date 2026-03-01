#!/usr/bin/env Rscript
# this handles the output of genrd2.
# there is metadata at the top so I use readLines() and not read.csv()
# and then parse the meta data to get over widht and height
library(ggplot2)
library(Cairo)

# pts <- read.csv("pbpts2.csv", header=F)
flines <- readLines("pbpts2.csv") ## File LINES
# flines <- readLines("r7pts.txt") ## File LINES

# yes, this looks awkward, but these are special files.
metadims <- gsub("# width=(\\d+) height=(\\d+)$", "\\1 \\2", flines[1])

# note strplit - it will split strings, then has to be changed to ints.
wh <- as.integer(unlist(strsplit(metadims, split=" ")))
pts <- as.integer(unlist(strsplit(flines[-1], split=",")))
lpts <- length(pts)

hwrat <- wh[2]/wh[1]

df <- data.frame(X=pts[seq(1,lpts,2)], Y=pts[seq(2,lpts,2)])
# ma <- matrix(pts, ncol=2, byrow=T)

# Cairo image template
CairoPNG("ggpb0.png", 800, round(800*hwrat))
gg <- ggplot(df, aes(x=X, y=Y)) +
    xlim(0,wh[1]) +
    ylim(0,wh[2]) +
    geom_point()
show(gg)
dev.off()
