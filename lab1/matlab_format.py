#!/usr/bin/python

import sys
import re
from collections import OrderedDict

if len(sys.argv) != 3 :
  print('Usage: ' + sys.argv[0] + ' infile var_id')
  exit()

infile = sys.argv[1]
var_id = sys.argv[2]

f = open(infile, 'r')
indata = f.read()
f.close()

im = indata.split('Image: ')
im = im[1:]

data = OrderedDict()

for s in im:
  image_name = s[0:3]
  data[image_name] = OrderedDict()
  #print(s)
  radius = s.split('Radius: ')
  radius = radius[1:]
  for r in radius:
    time_start = r.find('Filtering took: ') + len('Filtering took: ')
    time_end = r[time_start:].find(' ') + time_start
    try :
      #print(int(r[0:2]))
      data[image_name][int(r[0:3])] = float(r[time_start:time_end])
    except ValueError:
      try :
        #print('asdf ' + int(r[0:2]))
        data[image_name][int(r[0:2])] = float(r[time_start:time_end])
      except ValueError:
        #print('asdf ' + int(r[0:2]))
        data[image_name][int(r[0])] = float(r[time_start:time_end])

plot = 'plot('
legend = 'legend(\''
for key,value in data.items():
  out1 =  key + '_' + var_id
  out2 =  key + '_' + var_id + '_radius'
  plot += out2 + ', ' + out1 + ', \'-x\', '
  out1 +=  ' = ['
  out2 +=  ' = ['
  legend += key + '\', \''
  for radius, f in value.items() :
    out1 += str(f) + ' '
    out2 += str(radius) + ' '
  out1 = out1[:-1]
  out1 += '];'
  out2 = out2[:-1]
  out2 += '];'
  print(out1)
  print(out2)

plot = plot[:-2] + ');'
print('figure();')
print(plot)
legend += 'Location\', \'NorthWest\');'
print(legend)
print("xlabel('Radius');\nylabel('Run time');")
print("title('');")


