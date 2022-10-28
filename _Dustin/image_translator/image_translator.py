    # This takes in an image and translates it into a single dimensional
    # array that displays properly on the snowboard.

    # to make this work ensure the working folder is the _Dustin folder

from PIL import Image
import fileinput
import binascii
import numpy
from numpy import concatenate, full, loadtxt

#image parameters
background_color = "black" # can be hex value or color string
center_image = True # boolean value. determines if image is centered
resize_image = False # boolean value. determine if image is resized to fit board
shift_horizontal = 0 # integer for how much to shift image in pixels (- left + right)
shift_vertical = 0 # integer for how much to shift image in pixels (- down + up)

#media imports
image_file = "med.jpg" # image name
image_path = "media/" # path to pull media from
save_path = "board_media/tests/" # path to save output array

#global variables
max_height = 20 # height of board at tip/tail
max_width = 113 # max length of board
height_center = 18 # height of board in center

def image_input():
    """open image to display on board"""
    image = Image.open(image_path+image_file, "r") # open image
    # image.show()
    width,height = image.size
    # print("width: ", width)
    # print("height: ", height)
    if height > height_center:
        ratio = max_height / height # get ratio between max height and current height
        print("Ratio: ", ratio)
        print("New width: ", int(ratio*width))
        image = image.resize((int(ratio*width),max_height),Image.Dither.NONE) # resize image
        width,height = image.size

    image.convert('RGB') # removes alpha channel
    # image.show()

    return image

def image_positioning(image_width, image_length):
    print("Image size: ", image_width, image_length)
    if center_image is True:
        center_verticle = int((max_height/2)-(image_length/2))
        center_horizontal = int((max_width/2)-(image_width/2))
        # center = (int((113/2)-(25/2)), 1) # starting at top left of both images
        image_location = (center_horizontal, center_verticle)

    else:
        image_location = (0,0)

    image_location = (image_location[0]+shift_horizontal, image_location[1]+shift_vertical*-1)

    return image_location

def assign_to_board(display_image):
    """Overlay wanted image over blank image that is max size of board"""
    image_width, image_length = display_image.size
    image_location = image_positioning(image_width, image_length)
    print("Image location: ", image_location)
    blank = Image.new(mode='RGB', size=(max_width,max_height), color=background_color) # set blank background
    blank.paste(display_image, image_location)
    full_image_tuples = blank.load()
    full_image = [0 for i in range(max_height*max_width)] # total amount of pixels
    count = 0
    for y in range(max_height):
        for x in range(max_width):
            r, g, b = full_image_tuples[x,y]
            full_image[count] = (b << 16) | (g << 8) | r
            # print(count, full_image[count])
            count = count+1

    blank.show()
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

    file = numpy.concatenate((row1,row2,row3,row4,row5,row6,row7,row8,row9,row10, row11,row12,row13,row14,row15,row16,row17,row18,row19,row20))
    length = len(row1)+len(row2)+len(row3)+len(row4)+len(row5)+len(row6)+len(row7)+len(row8)+len(row9)+len(row10)+len(row11)+len(row12)+len(row13)+len(row14)+len(row15)+len(row16)+len(row17)+len(row18)+len(row19)+len(row20)
    print("File Length: ", length)
    # print(len(file1))
    return file

def array_assign(row_number, pix_val, direction):
    """assigns pixel data from image to individual rows"""

    # print("Row Number: ", row_number)
    second_blank = 113 # set to value that will never happen

    # all rows that have 38 pixels
    if row_number == 1 or row_number == 20:
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
        row_count = row_count_max - 1
        for i in range(len(pix_val)): # iterate through all pixels in image
            if (i>=(row_number-1)*113) and (i<=row_number*113): # iterate to row of interest (113 leds per row)
                start_split = initial_blank + row_count_max + second_blank
                if (count > initial_blank and count <= initial_blank+row_count_max) or (count > start_split and count <= start_split + row_count_max):
                    row[row_count] = pix_val[i]
                    row_count=row_count-1
                    # print(row_count)
                count = count + 1

    return row


def save_data(data):
    """save final data to files"""
    file = open(save_path+"image.dat", "w+")

    for i in range(len(data)):
        file.write(str(data[i]))
        if i < len(data) - 1: # to prevent ","" at end of file
            file.write(",") # seperate data in file



display_image = image_input()
full_image = assign_to_board(display_image)
file = mask_and_array(full_image)
save_data(file)
print("Done!")
