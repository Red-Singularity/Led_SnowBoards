# a simple script to automatically put board media into a format that can be compiled into
# the esp32's memory.
# this will take in a folder location for the frame data as an input and produce a single
# arduino header file with an array declaration for each 
# everything is relative to the _Dustin folder

# file locations

import os

dirname = os.path.dirname(__file__) # get absolute path to this location

gif_location = os.path.join(dirname, '../board_media/') # location of board media
gif_folder = "rgb_rainbow/" # folder of board data to bring to header file
header_location = os.path.join(dirname, '../arduino/sketches/led_snowboard_v2/') # location of header file
header_name = "mario.h" # name of the header file

two_dimensional_array_name = "marioData"

if __name__ == "__main__":
    header = open(header_location+header_name, "w") # create a new file
    frames = 0

    for path in os.listdir(gif_location + gif_folder):# determine how many files there are in the folder
        # check if current path is a file
        if os.path.isfile(os.path.join(gif_location + gif_folder, path)):
            frames += 1

    print("number of files: {}".format(frames))

    # frames = 27

    header.write("const int32_t " + two_dimensional_array_name + "[][2090] = ")
    header.write("{\n")
    for i in range(frames): # iterate through board data files
        # read string from .dat file and assign to data
        with open(gif_location+gif_folder+"image{}.dat".format(i), "r") as file:
            data = file.read()
        # print(data)
        header.write("{" + data + "}")
        if i < frames-1:
            header.write(",\n")
        else:
            header.write("\n}; ")

