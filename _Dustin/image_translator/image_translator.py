""" This takes in an image and translates it into a single dimensional
    array that displays properly on the snowboard. """

from PIL import Image
import fileinput
import numpy
from numpy import full, loadtxt


image_file = "link_s.png"
image_path = "media/"

max_height = 18 # height of board in center

def image_input():
    """open image to display on board"""
    image = Image.open(image_path+image_file, "r") #open image
    image.show()
    width,height = image.size
    # print(width)
    ratio = max_height / height # get ratio between max height and current height
    print("Ratio: ", ratio)
    print("New width: ", int(ratio*width))
    image = image.resize((int(ratio*width),max_height),Image.Dither.NONE) # resize image
    width,height = image.size
    print(height)
    pix_val = list(image.getdata())#get rgb value from image
    # print(pix_val)
    image.show()

def save_data():
    """save final data to files"""
    file = open("board_media/translator_test/Half_One.dat", "w+")


image_input()
