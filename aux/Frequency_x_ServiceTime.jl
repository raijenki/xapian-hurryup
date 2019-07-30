x = [ 1.2, 1.4, 1.6, 1.8, 2.0, 2.2]
y = [ [13.33, 8.31, 6.26, 5.05, 4.23, 3.6], [ 21.51, 12.29, 9.11, 7.22, 6.024, 5.21 ]] # y for 95-percentile and 99-percentile
#y_99 = [ 21.51, 12.29, 9.11, 7.22, 6.024, 5.21 ]
fn = plot(x, y_95, xlabel="Frequency (GHz)", ylabel="Service Time (ms)", linestyle=:solid, markershape=:circle, label = ["95-percentile", "99-percentile"], fmt = :png, lw=3 )
