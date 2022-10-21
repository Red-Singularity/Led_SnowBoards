
# this code is exploring the possibility of simulating my led matrix snowboard

# the plan is to virtually recreate the snowboard as an image using PIL. This will also virtually recreate the
# inputs to the board and what should be displayed as a result
# a stretch goal will to be to also represent the top of the board
# this has the potential to be the start of a visual representation in a custom

from PIL import Image
import fileinput
import numpy
from numpy import full, loadtxt

array_size = 1045
count = 0

def get_input(file):
        """open and read .dat files and convert to integer arrays"""
        conversion = [0 for i in range(array_size)]
        # print(len(conversion))
        Half_One = open(file, "r")
        data = loadtxt(file, delimiter=",",unpack=False)
        # convert data to python standard integer array
        count = 0
        # print(array_size)
        for i in range(array_size):
                count = count + 1
                conversion[i] = int(data[i].astype(int))
                # print(count)
                # print(conversion[i])
        return conversion

def create_image():
        Board_Half_One = get_input("Half_One.dat")
        Board_Half_Two = get_input("Half_Two.dat")

        Image1 = Image.new('RGB', (113,10), 0x000000) # mode, size, color
        pixels1 = Image1.load() # Create the pixel map
        side = 1
        Half_One_Assigned = assign(Board_Half_One, side) # assign file data to rows of the image

        # print(Half_One_Assigned)
        for i in range(Image1.size[1]):
                # print(Image1.size[1]) # print x dimension
                for j in range(Image1.size[0]):
                        value = Half_One_Assigned[i][j]
                        pixels1[j,i] = value
        Image1 = Image1.resize((565,50))
        Image1.show()

        Image2 = Image.new('RGB', (113,10), 0x000000) # mode, size, color
        pixels2 = Image2.load() # Create the pixel map
        side = 2
        Half_Two_Assigned = assign(Board_Half_Two, side) # assign file data to rows of the image
        # print(Half_Two_Assigned)

        for i in range(Image2.size[1]):
                # print(Image1.size[1]) # print x dimension
                for j in range(Image2.size[0]):
                        value = Half_Two_Assigned[i][j]
                        pixels2[j,i] = value

        Image2 = Image2.resize((565,50))
        Image2.show()

        Image3 = concatonate(Image1, Image2)
        Image3.show()

def concatonate(im1, im2):
        """Concatonates two images on top of each other to create one image"""
        dst = Image.new('RGB', (im1.width, im1.height + im2.height))
        dst.paste(im1, (0, 0))
        dst.paste(im2, (0, im1.height))
        return dst

def assign(file_data, side):
        """Create several single dimensional arrays that are the length of the board
        that data can be assigned to appropriately"""

        count = 0 # data point in array to start at

        reverse = False
        row_data_tuple = assign_row_D38(file_data, count, reverse)
        row1 = row_data_tuple[0]
        count = row_data_tuple[1]
        # print(count)

        reverse = True
        row_data_tuple = assign_row_D109(file_data, count, reverse)
        row2 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = False
        row_data_tuple = assign_row_D111(file_data, count, reverse)
        row3 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = True
        row_data_tuple = assign_row_D111(file_data, count, reverse)
        row4 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = False
        row_data_tuple = assign_row_D111(file_data, count, reverse)
        row5 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = True
        row_data_tuple = assign_row_D113(file_data, count, reverse)
        row6 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = False
        row_data_tuple = assign_row_D113(file_data, count, reverse)
        row7 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = True
        row_data_tuple = assign_row_D113(file_data, count, reverse)
        row8 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = False
        row_data_tuple = assign_row_D113(file_data, count, reverse)
        row9 = row_data_tuple[0]
        count = row_data_tuple[1]

        # print("final")
        reverse = True
        row_data_tuple = assign_row_D113(file_data, count, reverse)
        row10 = row_data_tuple[0]
        count = row_data_tuple[1]

        full_matrix = [0 for i in range(10)] # initialize array of arrays

        if side == 1:
                #assign all single dimensional arrays to another array
                full_matrix[0] = row1
                full_matrix[1] = row2
                full_matrix[2] = row3
                full_matrix[3] = row4
                full_matrix[4] = row5
                full_matrix[5] = row6
                full_matrix[6] = row7
                full_matrix[7] = row8
                full_matrix[8] = row9
                full_matrix[9] = row10

        if side == 2:
                full_matrix[0] = row10
                full_matrix[1] = row9
                full_matrix[2] = row8
                full_matrix[3] = row7
                full_matrix[4] = row6
                full_matrix[5] = row5
                full_matrix[6] = row4
                full_matrix[7] = row3
                full_matrix[8] = row2
                full_matrix[9] = row1

        # print(full_matrix)
        return(full_matrix)




def assign_row_D38(file_data, count, reverse):
        """"assign data to row with 38 leds"""
        row_D38 = [0 for i in range(113)] # initialize 1D array

        split = False # use if row has data split
        length_data = 19 # amount of data that's placed
        length_blank = 3 # amount of blank spaces before data
        count = assign_data(length_data, length_blank, count, row_D38, file_data, split, reverse)
        # print(row_D38)

        split = True
        length_data = 19
        length_blank = 69 # nice
        count = assign_data(length_data, length_blank, count, row_D38, file_data, split, reverse)
        # print(row_D38)

        return row_D38, count

def assign_row_D109(file_data, count, reverse):
        """"assign data to row with 109 leds"""
        row_D109 = [0 for i in range(113)] # initialize 1D array

        split = False
        length_data = 109 # amount of data that's placed 109
        length_blank = 2 # amount of blank spaces before data 2

        count = assign_data(length_data, length_blank, count, row_D109, file_data, split, reverse)
        return row_D109, count

def assign_row_D111(file_data, count, reverse):
        """"assign data to row with 111 leds"""
        row_D111 = [0 for i in range(113)] # initialize 1D array

        split = False
        length_data = 111 # amount of data that's placed
        length_blank = 1 # amount of blank spaces before data

        count = assign_data(length_data, length_blank, count, row_D111, file_data, split, reverse)
        return row_D111, count

def assign_row_D113(file_data, count, reverse):
        """"assign data to row with 113 leds"""
        row_D113 = [0 for i in range(113)] # initialize 1D array

        split = False
        length_data = 113 # amount of data that's placed
        length_blank = 0 # amount of blank spaces before data
        count = assign_data(length_data, length_blank, count, row_D113, file_data, split, reverse)
        return row_D113, count

def assign_data(length_data, length_blank, count, row, file_data, split, reverse):
        """assigns single dimensional data array from files to appropriate point on image"""
        row_location = 0 # marks point in row we are writing to
        # print("split:", split)
        # print("reverse:", reverse)

        if split and reverse == True:
                print("split, Reverse")
                row_location = 91 # 113-22
        elif (split == True) and (reverse == False):
                row_location = 22
        elif (split == False) and (reverse == True):
                row_location = 113 - 1
        else:
                row_location = 0

        if reverse == True:
                for i in range (length_blank):
                        row[row_location] = 0 # assign pixels to 0
                        row_location = row_location - 1

                # print("Length data: ", length_data)
                for i in range (length_data):
                        # print(i)
                        row[row_location] = file_data[count] # assign data from file to row
                        row_location = row_location - 1
                        count = count + 1

        else:
                for i in range (length_blank):
                        row[row_location] = 0 # assign pixels to 0
                        row_location = row_location + 1

                # print("Length data: ", length_data)
                for i in range (length_data):
                        # print(i)
                        row[row_location] = file_data[count] # assign data from file to row
                        row_location = row_location + 1
                        count = count + 1

        # print("row location: ", row_location)
        return count

create_image()

