#!/bin/bash

<<COMMENT
for f in ../images/*.ppm
do
  file=${f/*\/*\//}
  echo "Image: $file"
  echo "Threads: 1"
  echo "Radius: 5"
  ./blurc 1 5 $f $file
  echo
  echo "Radius: 10"
  ./blurc 1 10 $f $file
  echo
  echo "Radius: 100"
  ./blurc 1 100 $f $file
  echo
  echo "Threads: 8"
  echo "Radius: 5"
  ./blurc 8 5 $f $file
  echo
  echo "Radius: 10"
  ./blurc 8 10 $f $file
  echo
  echo "Radius: 100"
  ./blurc 8 100 $f $file
  echo
done
COMMENT

for threads in 1 4 8 16
do
  for f in ../images/*.ppm
  do
    file=${f/*\/*\//}
    echo "Image: $file"
    echo "Threads: $threads"
    ./thresc $threads $f $file
    echo
  done
done
