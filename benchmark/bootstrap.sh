#!/bin/sh
LD_LIBRARY_PATH=. ./benchmark -m 100 && mv benchmarkdata.py $1
