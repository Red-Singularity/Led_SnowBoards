
# this code is exploring the possibility of simulating my led matrix snowboard

# the plan is to virtually recreate the snowboard as an image using PIL. This will also virtually recreate the
# inputs to the board and what should be displayed as a result
# a stretch goal will to be to also represent the top of the board
# this has the potential to be the start of a visual representation in a custom

# to make this work ensure the working folder is the _Dustin folder

from PIL import Image
from numpy import full, loadtxt
import os


# sim parameters
background = 0xFFFFFF # hex color for background color of final image

# file to simulate
media = "nyan/" # file to simulate
relative_path = "board_media/" # relative path from _Dustin Folder

# file to save final result to if gif
relative_path_gif = "sim_outputs/"
sim_gif_name = "nyan.gif"

#Global variables
array_size = 2090 # total amount of leds in system
count = 0

def create_image(frames_str):
        full_path = relative_path + media
        board_full = get_input(full_path+"image"+frames_str+".dat")
        print("Full Path: "+full_path+"image"+frames_str+".dat")

        Board = Image.new('RGB', (113,20), background) # mode, size, color
        # Board.show()
        pixels = Board.load() # Create the pixel map
        All_Assigned = assign(board_full) # assign file data to rows of the image

        print("Board Height: ", Board.size[0])
        print("Board width: ", Board.size[1])

        for i in range(Board.size[1]):
                for j in range(Board.size[0]):
                        value = All_Assigned[i][j]
                        pixels[j,i] = value
        Board = Board.resize((565,100), Image.NEAREST)
        return Board

def get_input(file):
        """open and read .dat files and convert to integer arrays"""
        conversion = [0 for i in range(array_size)]
        data = loadtxt(file, delimiter=",",unpack=False)
        # convert data to python standard integer array
        count = 0
        for i in range(array_size):
                count = count + 1
                conversion[i] = int(data[i].astype(int))
        return conversion

def assign(full_board):
        """Create several single dimensional arrays that are the length of the board
        that data can be assigned to appropriately"""

        count = 0 # data point in array to start at

        reverse = False
        row_data_tuple = assign_row_D38(full_board, count, reverse)
        row1 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = True
        row_data_tuple = assign_row_D109(full_board, count, reverse)
        row2 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = False
        row_data_tuple = assign_row_D111(full_board, count, reverse)
        row3 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = True
        row_data_tuple = assign_row_D111(full_board, count, reverse)
        row4 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = False
        row_data_tuple = assign_row_D111(full_board, count, reverse)
        row5 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = True
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row6 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = False
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row7 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = True
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row8 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = False
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row9 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        reverse = True
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row10 = row_data_tuple[0]
        count = row_data_tuple[1]
        print("Count: ",count)

        #Side 2
        ########################################################################
        # count = 0
        reverse = False
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row11 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = True
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row12 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = False
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row13 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = True
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row14 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = False
        row_data_tuple = assign_row_D113(full_board, count, reverse)
        row15 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = True
        row_data_tuple = assign_row_D111(full_board, count, reverse)
        row16 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = False
        row_data_tuple = assign_row_D111(full_board, count, reverse)
        row17 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = True
        row_data_tuple = assign_row_D111(full_board, count, reverse)
        row18 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = False
        row_data_tuple = assign_row_D109(full_board, count, reverse)
        row19 = row_data_tuple[0]
        count = row_data_tuple[1]

        reverse = True
        row_data_tuple = assign_row_D38(full_board, count, reverse)
        row20 = row_data_tuple[0]
        count = row_data_tuple[1]

        full_matrix = [0 for i in range(20)] # initialize array of arrays

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

        row_test = Image.new('RGB', (113,1), background) # 113 pixels x 1 pixel to test row
        pixels_test = row_test.load() # Create the pixel map
        for i in range(row_test.size[0]):
                pixels_test[i,0] = row10[i]
        # row_test.show()

        full_matrix[10] = row11
        full_matrix[11] = row12
        full_matrix[12] = row13
        full_matrix[13] = row14
        full_matrix[14] = row15
        full_matrix[15] = row16
        full_matrix[16] = row17
        full_matrix[17] = row18
        full_matrix[18] = row19
        full_matrix[19] = row20

        # print(full_matrix)
        return(full_matrix)




def assign_row_D38(file_data, count, reverse):
        """"assign data to row with 38 leds"""
        row_D38 = [background for i in range(113)] # initialize 1D array

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
        row_D109 = [background for i in range(113)] # initialize 1D array

        split = False
        length_data = 109 # amount of data that's placed 109
        length_blank = 2 # amount of blank spaces before data 2

        count = assign_data(length_data, length_blank, count, row_D109, file_data, split, reverse)
        return row_D109, count

def assign_row_D111(file_data, count, reverse):
        """"assign data to row with 111 leds"""
        row_D111 = [background for i in range(113)] # initialize 1D array

        split = False
        length_data = 111 # amount of data that's placed
        length_blank = 1 # amount of blank spaces before data

        count = assign_data(length_data, length_blank, count, row_D111, file_data, split, reverse)
        return row_D111, count

def assign_row_D113(file_data, count, reverse):
        """"assign data to row with 113 leds"""
        row_D113 = [background for i in range(113)] # initialize 1D array

        split = False
        length_data = 113 # amount of data that's placed
        length_blank = 0 # amount of blank spaces before data
        count = assign_data(length_data, length_blank, count, row_D113, file_data, split, reverse)
        return row_D113, count

def assign_data(length_data, length_blank, count, row, file_data, split, reverse):
        """assigns single dimensional data array from files to appropriate point on image"""
        row_location = 0 # marks point in row we are writing to
        # print("Row: ", row)

        if split and reverse == True:
                print("split, Reverse")
                row_location = 90 # 113-22
        elif (split == True) and (reverse == False):
                row_location = 22
        elif (split == False) and (reverse == True):
                row_location = 112
        else:
                row_location = 0

        if reverse == True:
                for i in range (length_blank):
                        row[row_location] = background # assign pixels to background color
                        row_location = row_location - 1

                # print("Length data: ", length_data)
                for i in range (length_data):
                        # print(i)
                        row[row_location] = file_data[count] # assign data from file to row
                        row_location = row_location - 1
                        count = count + 1

        else:
                for i in range (length_blank):
                        row[row_location] = background # assign pixels to background color
                        row_location = row_location + 1

                # print("Length data: ", length_data)
                for i in range (length_data):
                        # print(i)
                        row[row_location] = file_data[count] # assign data from file to row
                        row_location = row_location + 1
                        count = count + 1

        # print("row location: ", row_location)
        return count

if __name__ == "__main__":
        full_gif = []

        frames = 0
        file_location = relative_path + media
        for path in os.scandir(file_location):
                if path.is_file():
                        frames = frames + 1

        frames = frames
        if frames == 0:
                frames = 1

        print("Frame Total: ", frames)

        frames_str = str(frames)

        for i in range(frames):
                print("works")
                Board = create_image(str(i))
                full_gif.append(Board)

        if frames > 0:
                # print(full_gif)
                full_gif[0].save(relative_path_gif + sim_gif_name,
                save_all=True, append_images=full_gif[1:], optimize=False, duration=1, loop=0)

        Board.show()

