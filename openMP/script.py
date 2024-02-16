
import os, sys

filename = "GA.cpp"
thread_counts = (1,2,4,6,8,10,12,14,16,18,20,24,32,40,48,64,80,96,128)

for each in thread_counts:
    os.system(f"g++ -fopenmp {filename} && export OMP_NUM_THREADS={each} && ./a.out >> result.txt")

