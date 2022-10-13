
# this code is exploring the possibility of simulating my led matrix snowboard

# the plan is to virtually recreate the snowboard as an image using PIL. This will also virtually recreate the
# inputs to the board and what should be displayed as a result
# a stretch goal will to be to also represent the top of the board
# this has the potential to be the start of a visual representation in a custom

from PIL import Image

sim = Image.new( 'RGB', (20,114), "black") # Create a new black image that has the size of ideal snowboard image
pixels = sim.load() # Create the pixel map
for i in range(sim.size[0]):    # For every pixel:
     for j in range(sim.size[1]):
        pixels[i,j] = (i, j, 100) # Set the colour accordingly

sim.show()