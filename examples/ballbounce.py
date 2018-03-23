#!/usr/bin/env python

# show a bouncing pokeball to demonstrate MPW

import sys
import time
import base64
from subprocess import Popen, PIPE, STDOUT

width = 200
height = 200

# ./pokeball.png dimensions 12x12
pokeball_png = '////////////////AAAAAAAAAAAAAAAA////////////////////////AAAAAAAAvwAAvwAAvwAAvwAAAAAAAAAA////////////AAAAvwAAvwAA3gAHvwAAvwAAvwAAvwAAvwAAAAAA////////AAAAvwAA3gAH////3gAHvwAAvwAAvwAAvwAAAAAA////AAAAvwAAvwAAvwAA3gAHvwAAvwAAvwAAvwAAvwAAvwAAAAAAAAAAvwAAvwAAvwAAvwAAAAAAAAAAvwAAvwAAvwAAvwAAAAAAAAAAAAAAvwAAvwAAAAAA////zMzMAAAAvwAAvwAAAAAAAAAAAAAA////AAAAAAAAAAAAzMzMzMzMAAAAAAAAAAAA2dnZAAAA////AAAA////////////AAAAAAAA2dnZ2dnZ2dnZAAAA////////AAAA2dnZ////////////2dnZ2dnZ2dnZ2dnZAAAA////////////AAAAAAAA2dnZ2dnZ2dnZ2dnZAAAAAAAA////////////////////////AAAAAAAAAAAAAAAA////////////////'

background = '\xFF\xFF\xFF'*width*height;
backgroundB64 = base64.b64encode(background)

pOpenObj = Popen(['./mpw', '--title', 'Ball Bounce', '--fps', '--width', str(width), '--height', str(height)], stdout=PIPE, stdin=PIPE)

x = 0
y = 0

vector = [3,5]

while 1:
	#time.sleep(.1)

	cmd = 'INSERT 0,0 %d,%d %s\n' % (width, height, backgroundB64)
	#print 'sending %s' % cmd
	pOpenObj.stdin.write(cmd)

	#time.sleep(.1)

	cmd = 'INSERT %d,%d 12,12 %s\n' % (x, y, pokeball_png)
	#print 'sending %s' % cmd
	pOpenObj.stdin.write(cmd)
	
	#time.sleep(.1)

	pOpenObj.stdin.write('SHOW\n')

	if x + vector[0] < 0 or x + vector[0] + 12 >= width:
		vector[0] *= -1
	x = x + vector[0]

	if y + vector[1] < 0 or y + vector[1] + 12 >= width:
		vector[1] *= -1
	y = y + vector[1]

