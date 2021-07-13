#!/bin/bash

#usage: ./get_report.sh DAYSHEETFILE

#remove existing csvs
echo `rm *.csv`

#compile and link code into executable main
echo `g++ -std=c++17 -c main.h main.cpp row.cpp row.h provider.cpp provider.h clinic.cpp clinic.h`
echo `g++ -std=c++17 -lm main.o row.o provider.o clinic.o -o main`

#run code with given daysheet
echo `./main $1 $2 $3`