import os, sys

filename = "GA6.cpp"
process = (3,4,6,8,10,12,14,16,18,20,24,32,40,48,64,80,96,128)

for each in process:
    os.system(f"mpic++ {filename} && mpirun -np {each} ./a.out >> result.txt")

