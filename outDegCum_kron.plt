set key top right
set logscale x 10
#set format x "10^{%L}"
set mxtics 10
#set format y "10^{%L}"
set mytics 10
set grid
set xlabel "In-degree"
set ylabel "Count"
set tics scale 2
#set terminal png size 1000,800
#set output 'inDeg.Model.png'
plot 	"outDegC.model.tab" using 1:2 title "Model" with linespoints pt 6, "outDegC.Kron.tab" using 1:2 title "Kron" with linespoints pt 6, "outDegC.Kron_scaled128.tab" using 1:2 title "KronScaled128" with linespoints pt 6, "outDegC.Kron_scaled256.tab" using 1:2 title "KronScaled256" with linespoints pt 6, "outDegC.Kron_scaled512.tab" using 1:2 title "KronScaled512" with linespoints pt 6, "outDegC.Kron_scaled1024.tab" using 1:2 title "KronScaled1024" with linespoints pt 6, "outDegC.Kron_scaled2048.tab" using 1:2 title "KronScaled2048" with linespoints pt 6, "outDegC.Kron_scaled4096.tab" using 1:2 title "KronScaled4096" with linespoints pt 6
