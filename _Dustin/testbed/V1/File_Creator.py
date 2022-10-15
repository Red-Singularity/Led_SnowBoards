"""Simple File that outputs a .dat file with a single dimensional array all with the same value"""

import fileinput
import numpy

change_file = 2 # 1 or 2
color = 0x0000FF # set color for leds
array_size = 1140 + 1 # array size

board_data = [color for i in range(array_size)] # set array size to half of board size

# change what board file to export to
if change_file == 1:
    file = open("Half_One.dat", "w+")
else:
    file = open("Half_Two.dat", "w+")

#write data to file recursively to avoif square brackets
for i in range(array_size):
    file.write(str(board_data[i]))
    if i < array_size - 1: # to prevent ","" at end of file
        file.write(", ")

if change_file == 1:
    file = open("Half_One.dat", "r")
else:
    file = open("Half_Two.dat", "r")

print(file.read())
file.close()



