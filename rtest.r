require(fExtremes)
## gevSim -
# Simulate GEV Data, use default length n=1000
# x = gevSim(model = list(xi = 0.25, mu = 0 , beta = 1), n = 1000)
# head(x)
x <- scan("D:\\ITMO\\Graphs\\ExtrVal\\KronTest\\MaxDegModel.dat", what = integer())
# Fit GEV Data by Probability Weighted Moments:
fit = gevFit(x, type = "pwm")
sink("D:\\ITMO\\Graphs\\ExtrVal\\KronTest\\gevfit.dat")
cat(summary(fit))
sink()
#slot(fit, "Estimated parameters")