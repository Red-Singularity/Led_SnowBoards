# a simple script to automatically put board media into a format that can be compiled into
# the esp32's memory.
# this will take in a folder location for the frame data as an input and produce a single
# arduino header file with an array declaration for each 
# everything is relative to the _Dustin folder

# file locations

import os

gif_location = "../board_media/"
gif_folder = "pacman"
header_location = "../arduino/sketches/led_snowboard_v2"
header_name = "gif_data.h"

if __name__ == "__main__":
    header = open(header_location+header_name, "w") # create a new file
    frames = # determine how many files there are in the folder

    for i in range(frames): # iterate through board data files
        # read string from .dat file and assign to data
        data = open(gif_location+gif_folder+"image{}.dat".format(i), "r")
        header.write("const int32_t pacmanFrameData{}[] = {".format(i)) # write array declaration
        header.write(data) # write data from .dat file
        header.write("}\n\n") # write end of array declaration with new lines
        