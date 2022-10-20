"""Simple File that outputs a .dat file with a single dimensional array all with the same value"""

import fileinput
import numpy

array_size = 1045 + 1 # array size
file = open("Half_One.dat", "w+")
color_step = 0xFF /array_size # 16777215 is the max color value divide by array_size

print(color_step)

#write data to file recursively to first 36 points
board_data = [0xFF0000 for i in range(array_size)] # set data in array
for i in range(0, array_size):
    # board_data[i] = int(i * color_step)
    file.write(str(board_data[i]))
    if i < array_size - 1: # to prevent ","" at end of file
        file.write(", ")

file = open("Half_One.dat", "r")

print(file.read())
file.close()



