#!/bin/sh
cd build
make
cd ..
./build/test > planner.dat
gnuplot -p planner.plot
