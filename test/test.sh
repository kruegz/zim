#!/bin/bash

# Script for testing features of zim

echo "Testing basic features"
echo "\n"

# Loop through all test files
for i in `seq 1 5`;
do
	# Feed file into zim via stdin
	../bin/zim < "in/${i}.txt" >> out.log

	# Check to see if output matches expected output
	diff "test" "out/${i}.txt"
	
	# Check return of diff to see if files were the same
	if [ $? -eq 0 ]
	then
		echo "Test ${i} success"
	else
		echo "Test ${i} fail"
	fi

done

echo "\n"
echo "Testing load files"
echo "\n"

# Loop through all test files
for i in `seq 1 4`;
do
	# Load file in zim and feed input over stdin
	../bin/zim "load/load${i}.txt" < "in/load${i}.txt" >> out.log

	diff "test" "out/load${i}.txt"

	# Check return of diff to see if files were the same
	if [ $? -eq 0 ]
	then
		echo "Load file ${i} test success"
	else
		echo "Load file ${i} test fail"
	fi

done

echo "\n"

# Remove test file
rm test
