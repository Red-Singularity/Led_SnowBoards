"""Simple File that outputs a .dat file with a single dimensional array all with the same value"""

import fileinput
import numpy

count = 0
array_size = 1045 # array size

save_path = "board_media/tests/" # path to save output array


file = open(save_path+"Half_One.dat", "w+")

#write data to file recursively to first 36 points
board_data = [0x000000 for i in range(array_size)] # set data in array
for i in range(38):
    color_step = 0x0000FF/38
    gradiant = int(i * color_step)
    board_data[i] = gradiant
    file.write(str(board_data[i]))
    file.write(", ")
    count = count + 1

shift = 1
count_init = count
for i in range(count, count+109):
    # print(i - count_init)
    color_step = 0x0000FF/109
    gradiant = int((i-count_init) * color_step) << (shift*8)
    board_data[i] = gradiant
    file.write(str(board_data[i]))
    file.write(", ")
    count = count + 1

shift = 2
for j in range(3):
    if shift > 2:
        shift = 0
    count_init = count
    print(shift)
    for i in range(count, count+111):
        # print(i - count_init)
        color_step = 0x0000FF/111
        gradiant = int((i-count_init) * color_step) << (shift*8)
        board_data[i] = gradiant
        file.write(str(board_data[i]))
        file.write(", ")
        count = count + 1
    shift = shift+1


shift = 2
for j in range(5):
    if shift > 2:
        shift = 0
    count_init = count
    print(shift)
    for i in range(count, count+113):
        # print(i - count_init)
        color_step = 0x0000FF/113
        # gradiant = 0x0000FF
        gradiant = int((i-count_init) * color_step) << (shift*8)
        board_data[i] = gradiant
        file.write(str(board_data[i]))
        if count < array_size-1: # to prevent ","" at end of file
            file.write(", ")
        count = count + 1
    shift = shift+1


file = open(save_path+"Half_One.dat", "r")
print(file.read())
file.close()



