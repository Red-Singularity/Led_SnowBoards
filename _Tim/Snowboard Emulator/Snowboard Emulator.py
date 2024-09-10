'''
Snowboard Emulator

    Description:
        This script takes an image or gif along with dimensions for the pixel count
        of the snowboard and displays a mock-up of the animation on the snowboard
    
    Use:
        Adjust the screen resolution parameter
        Adjust the filename parameter
        Run the script

    Notes:
        The display constants are also adjustable
    
'''
from PIL import Image, ImageOps, ImageChops, ImageFilter
import math

#Change these values
led_res = 80                        #linear resolution of the screen in leds/m
input_filename = "Nyan Cat 2.gif"   #filename of image to use

#Display constants
led_size_m = 0.002                      #size of the LED side length in m
board_length_m = 1.4                    #length of the snowboard from tail to tip in m
mask_filename = "140cm Profile.png"     #filename of the BW mask of the snowboard profile
diffusivity = 0.9                       #value for the amount that light will blend between adjacent pixels, must be > 0 with higher numbers being more blur

#-----Load the board profile mask and relate the real size of components to their image size-----
board_mask = Image.open(mask_filename)
board_mask = board_mask.convert('1')
board_mask = ImageOps.invert(board_mask)

mask_length_p, mask_width_p = board_mask.size   #size of the board profile mask in pixels

pixel_size = board_length_m / mask_length_p     #size of a pixel in meters

led_size_p = led_size_m / pixel_size            #size of the LED side length in pixels
if led_size_p < 1:
    raise Exception("Impossible LED size")

led_gap_m = (1 / led_res) - led_size_m
led_gap_p = led_gap_m / pixel_size              #distance between edges of neighbouring LEDs in pixels
if led_gap_p < 1:
    raise Exception("Impossible LED density")

Lres = math.floor((mask_length_p + led_gap_p) / (led_gap_p + led_size_p))   #resolution across the length of the snowboard at its longest point

Wres = math.floor((mask_width_p + led_gap_p) / (led_gap_p + led_size_p))    #number of LEDs along the width of the board at its widest point

#--------------------Create a mask with all the LED locations marked in white--------------------
led_mask = Image.new('L', (mask_length_p,mask_width_p), color = 0)
led_pixel_dat = led_mask.load()
board_pixel_dat = board_mask.load()

l_count: int
if Lres % 2 != 0:
    lstart_m = 0
    l_count = math.floor(Lres / 2) + 1
else:
    lstart_m = ((led_gap_p + led_size_p) / 2) * pixel_size
    l_count = round(Lres / 2)

w_count: int
if Wres % 2 != 0:
    wstart_m = 0
    w_count = math.floor(Wres / 2) + 1
else:
    wstart_m = ((led_gap_p + led_size_p) / 2) * pixel_size
    w_count = round(Wres / 2)

#fill in all the LEDs on the bottom right of the image
lpos_p: int
wpos_p: int
led_count = 0
small_width_count = 0       #number of LEDs at the thinnest part of the width of the snowboard
for l_c in range(l_count):
    for w_c in range(w_count):
        #get the center of the LED
        lpos_m = lstart_m + (l_c * (led_gap_m + led_size_m))
        lpos_p = math.floor((lpos_m / pixel_size) + (mask_length_p / 2))
        wpos_m = wstart_m + (w_c * (led_gap_m + led_size_m))
        wpos_p = math.floor((wpos_m / pixel_size) + (mask_width_p / 2))
        
        #test if the full LED is within the board profile
        in_board = True
        for y in range(math.ceil(-led_size_p/2), math.ceil(-led_size_p/2) + math.floor(led_size_p) + 1):
            for x in range(math.ceil(-led_size_p/2), math.ceil(-led_size_p/2) + math.floor(led_size_p) + 1):
                if lpos_p + y < 0 or wpos_p + x < 0 or lpos_p + y >= mask_length_p or wpos_p + x >= mask_width_p:
                    in_board = False
                    break
                elif board_pixel_dat[lpos_p + y, wpos_p + x] == 0:
                    in_board = False

            if in_board == False:
                break
        
        #fill in LED pixels if in the board profile
        if in_board:
            #add circular blur
            
            led_count = led_count + 1
            for y in range(math.ceil(-led_size_p/2), math.ceil(-led_size_p/2) + math.floor(led_size_p) + 1):
                for x in range(math.ceil(-led_size_p/2), math.ceil(-led_size_p/2) + math.floor(led_size_p) + 1):
                    led_pixel_dat[lpos_p + y, wpos_p + x] = 255
            
            if l_c == math.floor(l_count / 2):
                small_width_count = small_width_count + 1

led_count = led_count * 4
if Lres % 2 != 0:
    led_count = led_count - Lres

if Wres % 2 != 0:
    small_width_count = small_width_count * 2 - 1
    led_count = led_count - small_width_count
else:
    small_width_count = small_width_count * 2

#mirror twice to fill in all the LEDs
for y in range(math.floor(mask_length_p / 2)):
    for x in range(math.floor(mask_width_p / 2), mask_width_p):
        led_pixel_dat[y, x] = led_pixel_dat[mask_length_p - y - 1, x]

for y in range(mask_length_p):
    for x in range(math.floor(mask_width_p)):
        led_pixel_dat[y, x] = led_pixel_dat[y, mask_width_p - x - 1]

#---------------------Add the display image and light diffusion between LEDs---------------------
if input_filename.lower().endswith('.gif'):
    disp_gif = Image.open(input_filename)
    
    #do image analysis for each frame in gif
    frames = []
    for frame in range(disp_gif.n_frames):
        disp_gif.seek(frame)
        disp_im = disp_gif.copy().convert('RGB')

        disp_im = disp_im.resize((Lres, Wres), Image.NEAREST)
        disp_im = disp_im.resize(led_mask.size, Image.NEAREST)

        disp_im = Image.composite(disp_im, Image.new('RGB', led_mask.size, (0, 0, 0)), led_mask)
        prev_disp_im = disp_im.copy()

        disp_im = disp_im.filter(ImageFilter.BoxBlur(led_size_p * diffusivity))
        disp_im = Image.composite(prev_disp_im, disp_im, led_mask)

        frames.append(disp_im)
    
    #save result gif
    frames[0].save('processed_' + input_filename, save_all=True, append_images=frames[1:], loop=0)


elif input_filename.lower().endswith('.png', '.jpg', '.jpeg'):
    disp_im = Image.open(input_filename)
    disp_im = disp_im.resize((Lres, Wres), Image.NEAREST)
    disp_im = disp_im.resize(led_mask.size, Image.NEAREST)

    disp_im = Image.composite(disp_im, Image.new('RGB', led_mask.size, (0, 0, 0)), led_mask)
    prev_disp_im = disp_im.copy()

    disp_im = disp_im.filter(ImageFilter.BoxBlur(led_size_p * diffusivity))
    disp_im = Image.composite(prev_disp_im, disp_im, led_mask)
    disp_im.show()
else:
    raise Exception("Input file extension not valid")

print("Max LEDs along length: " + str(Lres))
print("Max LEDs along width: " + str(Wres))
print("Min LEDs along width: " + str(small_width_count))
print("LED resolution: " + str(1/(led_gap_m + led_size_m)) + "LEDs per meter")
print("Total LEDs: " + str(led_count))