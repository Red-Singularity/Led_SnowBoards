
# this code is exploring the possibility of simulating my led matrix snowboard

# the plan is to virtually recreate the snowboard as an image using PIL. This will also virtually recreate the
# inputs to the board and what should be displayed as a result
# a stretch goal will to be to also represent the top of the board
# this has the potential to be the start of a visual representation in a custom

from PIL import Image
import fileinput
from numpy import loadtxt

array_size = 1140 + 1

def get_input_one():
        """open and read .dat files and convert to integer arrays"""
        Half_One = open("Half_One.dat", "r")
        Board_Half_One = loadtxt("Half_One.dat", delimiter=",",unpack=False)
        Board_Half_One = Board_Half_One.astype(int)
        return Board_Half_One

def get_input_two():
        """open and read .dat files and convert to integer arrays"""
        Half_Two = open("Half_Two.dat", "r")
        Board_Half_Two = loadtxt("Half_Two.dat", delimiter=",",unpack=False)
        Board_Half_Two = Board_Half_Two.astype(int)
        return Board_Half_Two

def create_image():
        Board_Half_One = get_input_one()
        Board_Half_Two = get_input_two()

        Image1 = Image.new( 'RGB', (114,10), 0x000000) # mode, size, color
        pixels1 = Image1.load() # Create the pixel map
        for i in range(Image1.size[0]):
                for j in range(Image1.size[1]):
                        pixels1[i,j] = (i, j, Board_Half_One[i])
        Image1.show()

        Image2 = Image.new( 'RGB', (114,10), 0x000000) # mode, size, color
        pixels1 = Image2.load() # Create the pixel map

        Image3 = concatonate(Image1, Image2)
        Image3.show()

def concatonate(im1, im2):
        """Concatonates two images on top of each other to create one image"""
        dst = Image.new('RGB', (im1.width, im1.height + im2.height))
        dst.paste(im1, (0, 0))
        dst.paste(im2, (0, im1.height))
        return dst

create_image()

