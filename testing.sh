#!/bin/bash

#Mick Tarsel
# Runs tests for gridadpp.c 
# and saves into directory
#   ./testing/thread-count/
#   ./testing/grid-size/

#Arguments for gridapp.c:
# grid size, thread count

#
# outputs .csv files in ./testing/thread-count or ./tesining/grid-size like so:
#   [compile flag]_diff_[thread_count | grid_size].csv
#
#For example:
#   grid_diff_thread_count.csv 
#   was compiled with grid size = 10, and executed with 1-10 thread count 
#   with locking on grid
#

START=$(date +%s)

mkdir -p ./testing/thread-count/
mkdir -p ./testing/grid-size/

clear

echo "Sup, $USER"

echo ""
pwd
echo ""

#GRID : THREAD TESTING
for i in 1 2 3 4 5 6 7 8 9 10; do
echo "Executing with $i number of threads. Grid size 10 "
	./gridapp 10 $i -grid | grep elapsed | cut -d' ' -f4 >> ./testing/thread-count/grid_diff_thread_count.csv
	#Thanks to ddemarco5
done

#GRID: GRID TESTING
for i in 2 3 4 5 6 7 8 9 10; do
	echo "Executing with  grid size $i and 10 number of threads. "
	./gridapp $i 10 -grid | grep elapsed | cut -d' ' -f4 >> ./testing/grid-size/grid_diff_grid_size.csv
done



echo""
echo""
echo "FINISHED EXECUTING: "
echo""
echo " ./gridapp 10 [1-10] -grid " 
echo " ./gridapp [1-10] 10 -grid " 
echo "#####################"
echo""
echo""


#CELL : THREAD TESTING
for i in 1 2 3 4 5 6 7 8 9 10; do
echo "Executing with $i number of threads. Grid size 10 "
	./gridapp 10 $i -cell | grep elapsed | cut -d' ' -f4 >> ./testing/thread-count/cell_diff_thread_count.csv
done

#CELL : GRID TESTING
for i in 2 3 4 5 6 7 8 9 10; do
	echo "Executing with  grid size $i and 10 number of threads. "
	./gridapp $i 10 -cell | grep elapsed | cut -d' ' -f4 >> ./testing/grid-size/cell_diff_grid_size.csv
done


echo""
echo""
echo "FINISHED EXECUTING: "
echo""
echo " ./gridapp 10 [1-10] -cell " 
echo " ./gridapp [1-10] 10 -cell " 
echo "#####################"
echo""

#ROW : THREAD TESTING
for i in 1 2 3 4 5 6 7 8 9 10; do
echo "Executing with $i number of threads. Grid size 10 "
	./gridapp 10 $i -row | grep elapsed | cut -d' ' -f4 >> ./testing/thread-count/row_diff_thread_count.csv
done

#ROW: GRID TESTING
for i in 2 3 4 5 6 7 8 9 10; do
	echo "Executing with  grid size $i and 10 number of threads. "
	./gridapp $i 10 -row | grep elapsed | cut -d' ' -f4 >> ./testing/grid-size/row_diff_grid_size.csv
done


echo""
echo""
echo "FINISHED EXECUTING: "
echo""
echo " ./gridapp 10 [1-10] -row " 
echo " ./gridapp [1-10] 10 -row " 
echo "#####################"
echo""
echo""
echo " done."
echo""

#NONE : THREAD TESTING
for i in 1 2 3 4 5 6 7 8 9 10; do
echo "Executing with $i number of threads. Grid size 10 "
	./gridapp 10 $i -none | grep elapsed | cut -d' ' -f4 >> ./testing/thread-count/none_diff_thread_count.csv
done

#NONE: GRID TESTING
for i in 2 3 4 5 6 7 8 9 10; do
	echo "Executing with  grid size $i and 10 number of threads. "
	./gridapp $i 10 -none | grep elapsed | cut -d' ' -f4 >> ./testing/grid-size/none_diff_grid_size.csv
done

END=$(date +%s)

echo""
echo""
echo "FINISHED EXECUTING: "
echo""
echo " ./gridapp 10 [1-10] -none " 
echo " ./gridapp [1-10] 10 -none " 
echo "#####################"
echo""
echo""
DIFF=$(( $END - $START ))
echo "It took $DIFF seconds"
echo""
echo" done."
