    # This takes in an image and translates it into a single dimensional
    # array that displays properly on the snowboard.

    # to make this work ensure the working folder is the _Dustin folder

from PIL import Image
from pathlib import Path
import numpy
from numpy import concatenate, full, loadtxt
from pip import main

#image parameters
background_color = "blue" # can be hex value or color string
center_image = True # boolean value. determines if image is centered
resize_image = True # boolean value. determine if image is resized to fit board
rotate = 90 # integer representing angle image is rotated
shift_horizontal = 0 # integer for how much to shift image in pixels (- left + right)
shift_vertical = 0 # integer for how much to shift image in pixels (- down + up)

#media imports
image_file = "mememe.gif" # image name
image_path = "media/" # path to pull media from

save_path = "board_media/mememe/" # path to save output array

#global variables
max_height = 20 # height of board at tip/tail
max_width = 113 # max length of board
height_center = 18 # height of board in center

def image_input(image):
    """open image to display on board"""
    if rotate != 0:
        print("Rotating")
        image = image.rotate(rotate, Image.NEAREST, expand=1)
    width,height = image.size
    # print("width: ", width)
    # print("height: ", height)
    if resize_image == True and height > height_center:
        ratio = max_height / height # get ratio between max height and current height
        # print("Ratio: ", ratio)
        # print("New width: ", int(ratio*width))
        image = image.resize((int(ratio*width),max_height),Image.NEAREST) # resize image
        width,height = image.size

    # image.convert('RGB')
    # image.show()

    return image

def image_positioning(image_width, image_length):
    # print("Image size: ", image_width, image_length)
    if center_image is True:
        center_verticle = int((max_height/2)-(image_length/2))
        center_horizontal = int((max_width/2)-(image_width/2))
        image_location = (center_horizontal, center_verticle)

    else:
        image_location = (0,0)

    image_location = (image_location[0]+shift_horizontal, image_location[1]+shift_vertical*-1)

    return image_location

def assign_to_board(display_image):
    """Overlay wanted image over blank image that is max size of board"""
    # display_image.show()

    image_width, image_length = display_image.size
    image_location = image_positioning(image_width, image_length)
    print("Image location: ", image_location)
    blank = Image.new(mode='RGB', size=(max_width,max_height), color=background_color) # set blank background
    blank.convert('RGBA')
    blank.paste(display_image, image_location)

    # blank.show()
    full_image_tuples = blank.load()
    full_image = [0 for i in range(max_height*max_width)] # total amount of pixels
    count = 0
    for y in range(max_height):
        for x in range(max_width):
            r, g, b = full_image_tuples[x,y]
            full_image[count] = (b << 16) | (g << 8) | r
            # print(count, full_image[count])
            count = count+1

    # blank.show()
    return full_image

def mask_and_array(full_image):
    """Assign full image's data to 1d array that takes away unused pixels"""
    # print(len(full_image))

    row1 = array_assign(1, full_image, 0) # forward
    row2 = array_assign(2, full_image, 1) # reverse
    row3 = array_assign(3, full_image, 0) # forward
    row4 = array_assign(4, full_image, 1) # reverse
    row5 = array_assign(5, full_image, 0) # forward
    row6 = array_assign(6, full_image, 1) # reverse
    row7 = array_assign(7, full_image, 0) # forward
    row8 = array_assign(8, full_image, 1) # reverse
    row9 = array_assign(9, full_image, 0) # forward
    row10 = array_assign(10, full_image, 1) # reverse

    row11 = array_assign(11, full_image, 0) # forward
    row12 = array_assign(12, full_image, 1) # reverse
    row13 = array_assign(13, full_image, 0) # forward
    row14 = array_assign(14, full_image, 1) # reverse
    row15 = array_assign(15, full_image, 0) # forward
    row16 = array_assign(16, full_image, 1) # reverse
    row17 = array_assign(17, full_image, 0) # forward
    row18 = array_assign(18, full_image, 1) # reverse
    row19 = array_assign(19, full_image, 0) # forward
    row20 = array_assign(20, full_image, 1) # reverse
    # print(row20)

    file = numpy.concatenate((row1,row2,row3,row4,row5,row6,row7,row8,row9,row10,row11,row12,row13,row14,row15,row16,row17,row18,row19,row20))
    length = len(row1+row2+row3+row4+row5+row6+row7+row8+row9+row10+row11+row12+row13+row14+row15+row16+row17+row18+row19+row20)
    # print("File Length: ", length)
    # print(len(file1))
    return file

def array_assign(row_number, pix_val, direction):
    """assigns pixel data from image to individual rows"""

    # print("Row Number: ", row_number)
    second_blank = 114 # set to value that will never happen

    # all rows that have 38 pixels
    if row_number == 1:
        # print(row_number)
        row_count_max = 19
        initial_blank = 3 # beginning blank leds
        second_blank = 69 # nice

    elif row_number == 20:
        # print(row_number)
        row_count_max = 19
        initial_blank = 3 # beginning blank leds
        second_blank = 69 # nice

    # all rows that have 109 pixels
    elif row_number == 2 or row_number == 19:
        row_count_max = 109
        initial_blank = 2 # beginning blank leds

    # all rows that have 111 pixels
    elif (6 > row_number >  2) or (19 > row_number > 15):
        row_count_max = 111
        initial_blank = 1 # beginning blank leds

    # all other rows have 113 pixels
    else:
        row_count_max = 113
        initial_blank = 0

    #in order to make split rows compatable need to double row_count_max
    if row_number == 1 or row_number == 20:
        row = [255 for i in range(row_count_max*2)] # initialize row array
    else:
        row = [255 for i in range(row_count_max)] # initialize row array

    count = 0

    if direction == 0: # forward
        row_count = 0
        for i in range(len(pix_val)): # iterate through all pixels in image
            if (i>=(row_number-1)*113) and (i<=row_number*113): # iterate to row of interest (113 leds per row)
                start_split = initial_blank + row_count_max + second_blank
                # print("Count: ", count)
                if (count > initial_blank and count <= initial_blank+row_count_max) or (count > start_split and count <= start_split + row_count_max):
                    # print("Row Count: ", row_count)
                    row[row_count] = pix_val[i]
                    row_count=row_count+1
                count = count + 1 # count in if statement marks image row placement

    elif direction == 1: # reverse
        if row_number == 20:
            row_count = (row_count_max * 2) - 1
        else:
            row_count = row_count_max - 1
        for i in range(len(pix_val)): # iterate through all pixels in image
            if (i>=(row_number-1)*113) and (i<=row_number*113): # iterate to row of interest (113 leds per row)
                start_split = initial_blank + row_count_max + second_blank
                # print("Count: ", count)
                if (count > initial_blank and count <= initial_blank+row_count_max) or (count > start_split and count <= start_split + row_count_max):
                    # print("Row Count: ", row_count)
                    row[row_count] = pix_val[i]
                    row_count=row_count-1
                count = count + 1

    return row


def save_data(data, count):
    """save final data to files"""
    file_number = str(count)
    Path(save_path).mkdir(parents=True, exist_ok=True)
    file = open(save_path+"image"+file_number+".dat", "w+")

    for i in range(len(data)):
        file.write(str(data[i]))
        if i < len(data) - 1: # to prevent ","" at end of file
            file.write(",") # seperate data in file


if __name__ == "__main__":
    count = 0
    image = Image.open(image_path+image_file, "r") # open image
    if image.format == "GIF":
        try:
            while(1): # infinite loop until out of gif frames
                print("Gif Identified")
                print("Count: ", count, "\n")
                image.seek(image.tell()+ 1)
                display_image = image_input(image)
                # display_image.show()
                full_image = assign_to_board(display_image)
                file = mask_and_array(full_image)
                save_data(file, count)
                count = count + 1
        except EOFError: # if there are no more frames, pass
            pass

    else:
        display_image = image_input(image) # returns image that has been modified to fit board
        display_image.show()

        full_image = assign_to_board(display_image) # returns array of image data as hex values
        file = mask_and_array(full_image) # masks image data to fit on board
        save_data(file, count)

    print("Done!")
