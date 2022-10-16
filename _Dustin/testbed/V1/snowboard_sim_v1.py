
# this code is exploring the possibility of simulating my led matrix snowboard

# the plan is to virtually recreate the snowboard as an image using PIL. This will also virtually recreate the
# inputs to the board and what should be displayed as a result
# a stretch goal will to be to also represent the top of the board
# this has the potential to be the start of a visual representation in a custom

from PIL import Image
import fileinput
from numpy import full, loadtxt

array_size = 1140 + 1
count = 0

def get_input(file):
        """open and read .dat files and convert to integer arrays"""
        Half_One = open(file, "r")
        data = loadtxt(file, delimiter=",",unpack=False)
        data = data.astype(int)
        return data

def create_image():
        Board_Half_One = get_input("Half_One.dat")
        Board_Half_Two = get_input("Half_Two.dat")

        Image1 = Image.new( 'RGB', (113,10), 0x000000) # mode, size, color
        pixels1 = Image1.load() # Create the pixel map
        Half_One_Assigned = assign(Board_Half_One) # assign file data to rows of the image
        for i in range(Image1.size[1]):
                # print(Image1.size[1])
                for j in range(Image1.size[0]):
                        # print(Image1.size[0])
                        print("test: ", type(Half_One_Assigned[i][j]))
                        # value = Half_One_Assigned[j][i]
                        # pixels1[i,j] = value

        # Image1.show()

        Image2 = Image.new( 'RGB', (113,10), 0x000000) # mode, size, color
        pixels1 = Image2.load() # Create the pixel map

        Image3 = concatonate(Image1, Image2)
        # Image3.show()

def concatonate(im1, im2):
        """Concatonates two images on top of each other to create one image"""
        dst = Image.new('RGB', (im1.width, im1.height + im2.height))
        dst.paste(im1, (0, 0))
        dst.paste(im2, (0, im1.height))
        return dst

def assign(file_data):
        """Create several single dimensional arrays that are the length of the board
        that data can be assigned to appropriately"""
        count = 0 # data point in array to start at
        row_data_tuple = assign_row_D38(file_data, count)
        row1 = row_data_tuple[0]
        # count = row_data_tuple[1]
        # print(count)
        count = 113

        row_data_tuple = assign_row_D109(file_data, count)
        row2 = row_data_tuple[0]
        # count = row_data_tuple[1]
        # print(row2)
        count = count + 113

        row_data_tuple = assign_row_D111(file_data, count)
        row3 = row_data_tuple[0]
        # count = row_data_tuple[1]
        # print(row3)
        count = count + 113

        row_data_tuple = assign_row_D111(file_data, count)
        row4 = row_data_tuple[0]
        # count = row_data_tuple[1]
        count = count + 113

        row_data_tuple = assign_row_D111(file_data, count)
        row5 = row_data_tuple[0]
        # count = row_data_tuple[1]
        count = count + 113

        row_data_tuple = assign_row_D113(file_data, count)
        row6 = row_data_tuple[0]
        # count = row_data_tuple[1]
        count = count + 113

        row_data_tuple = assign_row_D113(file_data, count)
        row7 = row_data_tuple[0]
        # count = row_data_tuple[1]
        count = count + 113

        row_data_tuple = assign_row_D113(file_data, count)
        row8 = row_data_tuple[0]
        # count = row_data_tuple[1]
        count = count + 113

        row_data_tuple = assign_row_D113(file_data, count)
        row9 = row_data_tuple[0]
        # count = row_data_tuple[1]
        count = count + 113

        row_data_tuple = assign_row_D113(file_data, count)
        row10 = row_data_tuple[0]
        # count = row_data_tuple[1]
        count = count + 113

        full_matrix = [0 for i in range(10)] # initialize array of arrays

        #assign all single dimensional arrays to another array
        full_matrix[0] = row1
        # print(row1)
        full_matrix[1] = row2
        full_matrix[2] = row3
        full_matrix[3] = row4
        full_matrix[4] = row5
        full_matrix[5] = row6
        full_matrix[6] = row7
        full_matrix[7] = row8
        full_matrix[8] = row9
        full_matrix[9] = row10

        # print(full_matrix)
        return(full_matrix)




def assign_row_D38(file_data, count):
        """"assign data to row with 39 leds"""
        row_D38 = [0 for i in range(113)] # initialize 1D array

        split = False # use if row is has data split
        length_data = 19 # amount of data that's placed
        length_blank = 3 # amount of blank spaces before data
        assign_data(length_data, length_blank, count, row_D38, file_data, split)
        # print(row_D38)

        split = True
        length_data = 19
        length_blank = 69 # nice
        assign_data(length_data, length_blank, count, row_D38, file_data, split)
        # print(row_D38)

        return row_D38, count

def assign_row_D109(file_data, count):
        """"assign data to row with 109 leds"""
        row_D109 = [0 for i in range(113)] # initialize 1D array

        split = False
        length_data = 109 # amount of data that's placed 109
        length_blank = 2 # amount of blank spaces before data 2

        assign_data(length_data, length_blank, count, row_D109, file_data, split)
        return row_D109, count

def assign_row_D111(file_data, count):
        """"assign data to row with 111 leds"""
        row_D111 = [0 for i in range(113)] # initialize 1D array

        split = False
        length_data = 111 # amount of data that's placed
        length_blank = 1 # amount of blank spaces before data

        assign_data(length_data, length_blank, count, row_D111, file_data, split)
        return row_D111, count

def assign_row_D113(file_data, count):
        """"assign data to row with 113 leds"""
        row_D113 = [0 for i in range(113)] # initialize 1D array

        split = False
        length_data = 113 # amount of data that's placed
        length_blank = 0 # amount of blank spaces before data

        assign_data(length_data, length_blank, count, row_D113, file_data, split)
        return row_D113, count

def assign_data(length_data, length_blank, count, row, file_data, split):
        """assigns single dimensional data array from files to appropriate point on image"""
        if split == True:
                # print("split")
                count_s = 22
                count_init = count
                for i in range (count_s, length_blank+count_s):
                        row[i-count_init] = 0 # assign pixels to 0
                        count_s = count_s + 1

                # print(len(file_data))
                for i in range (count_s, count_s+length_data):
                        row[i-count_init] = file_data[i] # assign data from file to row

        else:
                count_init = count
                for i in range (count, length_blank+count):
                        row[i-count_init] = 0 # assign pixels to 0
                        count = count + 1

                # print(len(file_data))
                for i in range (count, count+length_data):
                        row[i-count_init] = file_data[i] # assign data from file to row
                        count = count + 1

create_image()

