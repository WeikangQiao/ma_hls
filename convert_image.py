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

def resize_image(image, height, width,
        channels=None,
        resize_mode=None,
        ):
    """
    Resizes an image and returns it as a np.array

    Arguments:
    image -- a PIL.Image or numpy.ndarray
    height -- height of new image
    width -- width of new image

    Keyword Arguments:
    channels -- channels of new image (stays unchanged if not specified)
    resize_mode -- can be crop, squash, fill or half_crop
    """
    if resize_mode is None:
        resize_mode = 'squash'
    if resize_mode not in ['crop', 'squash', 'fill', 'half_crop']:
        print('resize_mode "%s" not supported' % resize_mode)
        exit(-1)
    if channels not in [None, 1, 3]:
        print('unsupported number of channels: %s' % channels)
        exit(-1)

    if isinstance(image, PIL.Image.Image):
        # Convert image mode (channels)
        if channels is None:
            image_mode = image.mode
            if image_mode == 'L':
                channels = 1
            elif image_mode == 'RGB':
                channels = 3
            else:
                print('unknown image mode "%s"' % image_mode)
                exit(-1)
        elif channels == 1:
            # 8-bit pixels, black and white
            image_mode = 'L'
        elif channels == 3:
            # 3x8-bit pixels, true color
            image_mode = 'RGB'
        if image.mode != image_mode:
            image = image.convert(image_mode)
        image = np.array(image)
    elif isinstance(image, np.ndarray):
        if image.dtype != np.uint8:
            image = image.astype(np.uint8)
        if image.ndim == 3 and image.shape[2] == 1:
            image = image.reshape(image.shape[:2])
        if channels is None:
            if image.ndim == 2:
                channels = 1
            elif image.ndim == 3 and image.shape[2] == 3:
                channels = 3
            else:
                print('invalid image shape: %s' % (image.shape,))
                exit(-1)
                
        elif channels == 1:
            if image.ndim != 2:
                if image.ndim == 3 and image.shape[2] == 3:
                    # color to grayscale
                    image = np.dot(image, [0.299, 0.587, 0.114]).astype(np.uint8)
                else:
                    print('invalid image shape: %s' % (image.shape,))
                    exit(-1)
                    
        elif channels == 3:
            if image.ndim == 2:
                # grayscale to color
                image = np.repeat(image,3).reshape(image.shape + (3,))
            elif image.shape[2] != 3:
                print('invalid image shape: %s' % (image.shape,))
                exit(-1)
                
    else:
        print('resize_image() expected a PIL.Image.Image or a numpy.ndarray')
        exit(-1)
        

    # No need to resize
    if image.shape[0] == height and image.shape[1] == width:
        return image

    ### Resize
    interp = 'bicubic'

    width_ratio = float(image.shape[1]) / width
    height_ratio = float(image.shape[0]) / height
    if resize_mode == 'squash' or width_ratio == height_ratio:
        return scipy.misc.imresize(image, (height, width), interp=interp)
    elif resize_mode == 'crop':
        # resize to smallest of ratios (relatively larger image), keeping aspect ratio
        if width_ratio > height_ratio:
            resize_height = height
            resize_width = int(round(image.shape[1] / height_ratio))
        else:
            resize_width = width
            resize_height = int(round(image.shape[0] / width_ratio))
        image = scipy.misc.imresize(image, (resize_height, resize_width), interp=interp)

        # chop off ends of dimension that is still too long
        if width_ratio > height_ratio:
            start = int(round((resize_width-width)/2.0))
            return image[:,start:start+width]
        else:
            start = int(round((resize_height-height)/2.0))
            return image[start:start+height,:]
    else:
        if resize_mode == 'fill':
            # resize to biggest of ratios (relatively smaller image), keeping aspect ratio
            if width_ratio > height_ratio:
                resize_width = width
                resize_height = int(round(image.shape[0] / width_ratio))
                if (height - resize_height) % 2 == 1:
                    resize_height += 1
            else:
                resize_height = height
                resize_width = int(round(image.shape[1] / height_ratio))
                if (width - resize_width) % 2 == 1:
                    resize_width += 1
            image = scipy.misc.imresize(image, (resize_height, resize_width), interp=interp)
        elif resize_mode == 'half_crop':
            # resize to average ratio keeping aspect ratio
            new_ratio = (width_ratio + height_ratio) / 2.0
            resize_width = int(round(image.shape[1] / new_ratio))
            resize_height = int(round(image.shape[0] / new_ratio))
            if width_ratio > height_ratio and (height - resize_height) % 2 == 1:
                resize_height += 1
            elif width_ratio < height_ratio and (width - resize_width) % 2 == 1:
                resize_width += 1
            image = scipy.misc.imresize(image, (resize_height, resize_width), interp=interp)
            # chop off ends of dimension that is still too long
            if width_ratio > height_ratio:
                start = int(round((resize_width-width)/2.0))
                image = image[:,start:start+width]
            else:
                start = int(round((resize_height-height)/2.0))
                image = image[start:start+height,:]
        else:
            raise Exception('unrecognized resize_mode "%s"' % resize_mode)

        # fill ends of dimension that is too short with random noise
        if width_ratio > height_ratio:
            padding = (height - resize_height)/2
            noise_size = (padding, width)
            if channels > 1:
                noise_size += (channels,)
            noise = np.random.randint(0, 255, noise_size).astype('uint8')
            image = np.concatenate((noise, image, noise), axis=0)
        else:
            padding = (width - resize_width)/2
            noise_size = (height, padding)
            if channels > 1:
                noise_size += (channels,)
            noise = np.random.randint(0, 255, noise_size).astype('uint8')
            image = np.concatenate((noise, image, noise), axis=1)

        return image              
              
              
                    
######
###### Open Image, Extract all Pixels
######

start_time = time.clock()

# Load Image from File or Web:
original_image = load_image(infile)

# Get Dimensions:
if resize > 0:
    width = height = resize
else:
    width, height = original_image.size
    
# Convert Image to NP array, resizing if necessary
resized_image = resize_image(original_image, width, height, resize_mode='half_crop')

# Check Dimensions
channels = resized_image.shape[2]
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
    
# Test Pixels at Start of Image
# resized_image[0,0,...] = [0,0,0]
# resized_image[0,1,...] = [255,255,255]
# resized_image[0,2,...] = [255,0,0]
# resized_image[0,3,...] = [0,255,0]
# resized_image[0,4,...] = [0,0,255]

# Reorder RGB -> BGR
resized_image[:,:,:] = resized_image[:,:,(2,1,0)]

PIL.Image.fromarray(resized_image).show()

# Reorder and write to linear Pixels list:
# NP image array shape: (height, width, channels)
print("Reorder Channels...")
pixels = []
for y in range(height):
    for x in range(width):
        for c in range(channels):  
            pixels.append(float(resized_image[y][x][c]))

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
