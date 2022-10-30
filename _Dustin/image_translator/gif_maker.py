"""Code for making a simple gif using pillow"""

from PIL import Image, ImageDraw
import fileinput
import numpy
from numpy import full, loadtxt

images = []

height = 20
width = 113
center_width = width // 2
center_height = height // 2
color_1 = (0, 0, 255)
color_2 = (0, 255, 0)
color_3 = (255, 0, 0)
max_radius = int(center_width * 1)
step = 5

for i in range(0, max_radius, step):
    im = Image.new('RGB', (width, height), color_1)
    draw = ImageDraw.Draw(im)
    draw.ellipse((center_width - i, center_height - i, center_width + i, center_height + i), fill=color_2)
    images.append(im)

for i in range(0, max_radius, step):
    im = Image.new('RGB', (width, width), color_2)
    draw = ImageDraw.Draw(im)
    draw.ellipse((center_width - i, center_height - i, center_width + i, center_height + i), fill=color_1)
    images.append(im)

images[0].save('media/gif_test.gif',
               save_all=True, append_images=images[1:], optimize=False, duration=.1, loop=0)
