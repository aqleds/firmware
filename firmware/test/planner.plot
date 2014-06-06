
set parametric

set trange [-1:1.5]

sunrise=7
day=11
sunset=19
night=23


set yr [-0.1:1.1]
plot \
"planner.dat" using 1:3 title "ch1" with lines, \
"planner.dat" using 1:4 title "ch2" with lines, \
"planner.dat" using 1:5 title "ch3" with lines, \
"planner.dat" using 1:6 title "Total" with lines, \
sunrise,t, \
day,t,     \
sunset,t,  \
night,t

