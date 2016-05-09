#!/usr/bin/env python2.7

#######
## convert_image.py
## (c) 2016 David Gschwend
##
## Usage: python2.7 convert_image.py input.{png,jpg} [image.img [resize]]
## Image is converted into correct format for FPGA memory.
## Default Output Filename: image.img
## Input File Name can also be a URL -> image is fetched from Internet.
## If "resize" (new image size) is specified, image will be resized to this dim.
#######

import os, sys
import time
import struct

import numpy as np
import PIL.Image
import scipy.misc
from urlparse import urlparse

infile = "undef"
outfile = "undef"
resize = 0
if len(sys.argv) == 2:
    infile = sys.argv[1]
    outfile = "image.img"
if len(sys.argv) == 3:
    infile = sys.argv[1]
    outfile = sys.argv[2]
if len(sys.argv) == 4:
    infile = sys.argv[1]
    outfile = sys.argv[2]
    resize = int(sys.argv[3])
    
if infile == "undef" or outfile == "undef": 
    print("Usage: %s input.{png,jpg} [image.img [size]]" % sys.argv[0])
    exit(-1)
    
print("Using input file: {}, output file: {}".format(infile, outfile))
if resize > 0: print("Resizing to {}x{}".format(resize, resize))

 
#####
#####  Functions from NVIDIA's DIGITS utils/*.py
#####   

SUPPORTED_EXTENSIONS = ('.png','.jpg','.jpeg','.bmp','.ppm')

def is_url(url):
    return url is not None and urlparse(url).scheme != "" and not os.path.exists(url)
              
def load_image(path):
    """
    Reads a file from `path` and returns a PIL.Image with mode 'L' or 'RGB'
    Raises LoadImageError

    Arguments:
    path -- path to the image, can be a filesystem path or a URL
    """
    image = None
    if is_url(path):
        try:
            r = requests.get(path,
                    allow_redirects=False,
                    timeout=HTTP_TIMEOUT)
            r.raise_for_status()
            stream = StringIO(r.content)
            image = PIL.Image.open(stream)
        except requests.exceptions.RequestException as e:
            print("Image Load Error (URL)")
            exit(-1)
        except IOError as e:
            print("Image Load Error (URL)")
            exit(-1)
    elif os.path.exists(path):
        try:
            image = PIL.Image.open(path)
            image.load()
        except IOError as e:
            print("Image Load Error (File Systems): %s" % e)
            exit(-1)
    else:
        print("Image Load Error (File Systems): File Not Found.")
        exit(-1)

    if image.mode in ['L', 'RGB']:
        # No conversion necessary
        return image
    elif image.mode in ['1']:
        # Easy conversion to L
        return image.convert('L')
    elif image.mode in ['LA']:
        # Deal with transparencies
        new = PIL.Image.new('L', image.size, 255)
        new.paste(image, mask=image.convert('RGBA'))
        return new
    elif image.mode in ['CMYK', 'YCbCr']:
        # Easy conversion to RGB
        return image.convert('RGB')
    elif image.mode in ['P', 'RGBA']:
        # Deal with transparencies
        new = PIL.Image.new('RGB', image.size, (255, 255, 255))
        new.paste(image, mask=image.convert('RGBA'))
        return new
    else:
        print("Image Load Error: Image mode '%s' not supported" % image.mode)
        exit(-1)
        
def load_and_process(path, height, width, mode='RGB'):
    """
    Load an image from disk or web

    Returns fully pre-processed image (channels x width x height)

    Arguments:
    path -- path to an image on disk or on web
    width -- resize dimension
    height -- resize dimension

    Keyword arguments:
    mode -- the PIL mode that the image should be converted to
        (RGB for color or L for grayscale)
    """
    image = load_image(path)
    image = image.convert(mode)
    PILimage = image
    image = np.array(image)
    
    # half-crop
    height_ratio = float(image.shape[0])/height
    width_ratio = float(image.shape[1])/width
    new_ratio = (width_ratio + height_ratio) / 2.0
    resize_width = int(round(image.shape[1] / new_ratio))
    resize_height = int(round(image.shape[0] / new_ratio))
    if width_ratio > height_ratio and (height - resize_height) % 2 == 1:
        resize_height += 1
    elif width_ratio < height_ratio and (width - resize_width) % 2 == 1:
        resize_width += 1
    #image = scipy.misc.imresize(image,(resize_height, resize_width),interp='bicubic')
    image = np.array(PILimage.resize((resize_width, resize_height), PIL.Image.LANCZOS))
    print 'resized shape: ', image.shape
    
    if width_ratio > height_ratio:
        start = int(round((resize_width-width)/2.0))
        image = image[:,start:start+width]
    else:
        start = int(round((resize_height-height)/2.0))
        image = image[start:start+height,:]
    
    # half-fill: fill ends of dimension that is too short with random noise
    if width_ratio > height_ratio:
        padding = (height - resize_height)/2
        noise_size = (padding, width, 3)
        noise = np.random.randint(0, 255, noise_size).astype('uint8')
        image = np.concatenate((noise, image, noise), axis=0)
    else:
        padding = (width - resize_width)/2
        noise_size = (height, padding, 3)
        noise = np.random.randint(0, 255, noise_size).astype('uint8')
        image = np.concatenate((noise, image, noise), axis=1)
    
    processed = np.zeros((3, width, height), np.float32)
    
    # Subtract Mean, Swap Channels RGB -> BGR, Transpose (W,H,CH) to (CH,W,H)
    #mean_rgb = [104,117,123]
    processed[0,:,:] = (image[:,:,2]-104.0)
    processed[1,:,:] = (image[:,:,1]-117.0)
    processed[2,:,:] = (image[:,:,0]-123.0)
        
    return processed


######
###### Open Image, Extract all Pixels
######

start_time = time.clock()

# Get Original Image to compare Dimensions:
original_image = load_image(infile)

# Get Dimensions:
if resize > 0:
    width = height = resize
else:
    width, height = original_image.size
    
# Load and Process Image (mean subtract, channel swap, dimension transpose)
# returns image as nparray (channels x height x width)
processed_image = load_and_process(infile, height, width)
    
# Check Dimensions
channels = processed_image.shape[0]
print "New Dimensions: {}x{}, {} Channels\n".format(width, height, channels)
if channels != 3:
    print "Expected 3 Channels! Aborting.\n"
    exit(-1)
if width != height:
    print "Expected Square Image! Aborting.\n"
    exit(-1)
if not (width in [1024, 512, 256, 128, 64, 32, 16, 8, 4, 2]):
    print "Expected Image with Side length 2^n! Aborting.\n"
    exit(-1)
    
# Display
# have to convert back to WxHxch format...
pilimage = np.zeros((height,width,3),np.uint8)
pilimage[:,:,0] = processed_image[0,:,:]/1.2 + 127
pilimage[:,:,1] = processed_image[1,:,:]/1.2 + 127
pilimage[:,:,2] = processed_image[2,:,:]/1.2 + 127
PIL.Image.fromarray(pilimage).show()

# Reorder and write to linear Pixels list:
# NP image array shape: (channels, width, height)
print("Reorder Channels...")
pixels = []
for y in range(height):
    for x in range(width):
        for c in range(channels):  
            pixels.append(float(processed_image[c][x][y]))

# Write weights to binary file
print("Write to Output File...")
floatstruct = struct.pack('f'*len(pixels), *pixels)
with open(outfile, "wb") as f:
    f.write(floatstruct)
    
end_time = time.clock()
time_taken = end_time - start_time

# Print Confirmation to stdout
print("\nDONE:\n-----------")
print("Image converted to {}x{} and saved as <{}>".format(width,height,outfile))
print("File Size: {} Bytes".format(os.path.getsize(outfile)))

print("Total Operations took %d seconds on CPU." % time_taken)

# ### 
