#!/bin/bash

for f in images/*.ppm
do
  file=${f/*\//output/}
  echo "Image: $file"
  echo "Threads: 1"
  ./thresc 1 $f $file
  echo
  echo
  echo "Threads: 8"
  ./thresc 8 $f $file
  echo
done
