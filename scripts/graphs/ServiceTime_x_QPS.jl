# This script should create the ServiceTime_x_QPS.png image

y = [2.216, 2.165, 866.260, 10845.570, 29261.411, 45981.357, 93677.102 ]; x = [1000, 2000, 4000, 6000, 8000, 10000, 12000]; 
fn = plot(x, y, xlabel="Queries Per Second (QPS)", ylabel="Service Time (ms)", linestyle=:solid, markershape=:circle, fmt = :png )
