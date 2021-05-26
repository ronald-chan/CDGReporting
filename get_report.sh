#!/bin/bash

#usage: ./get_report.sh DAYSHEETFILE

#remove existing csvs
echo `rm *summary.csv`

#create csvs needed
echo `touch CDGsummary.csv`
echo `touch CPDsummary.csv`

#compile and link code into executable main
echo `g++ -std=c++17 -c main.cpp row.cpp row.h`
echo `g++ -std=c++17 -lm main.o row.o -o main`

#run code with given daysheet
echo `./main $1`