#!/usr/bin/env python

# convert images to base64 strings to be used in MPW

import os
import re
import sys
import base64
import struct
from PIL import Image

if __name__ == '__main__':
	fpath = sys.argv[1]

	im = Image.open(fpath)
	im = im.convert('RGB')

	data = ''

	for y in range(im.height):
		for x in range(im.width):
			r,g,b = im.getpixel((x,y))
			data += struct.pack('BBB',r,g,b)

	print '# %s dimensions %dx%d' % (fpath, im.width, im.height)
	varName = re.sub(r'\W', '_', os.path.basename(fpath));
	print '%s = \'%s\'' % (varName, base64.b64encode(data))

