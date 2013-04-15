#!/bin/bash

for f in ../images/*.ppm
do
  file=${f/*\/*\//}
  echo "Image: $file"
  echo "Processes: 1"
  echo "Radius: 5"
  mpirun -np 1 ./mpiblurc 5 $f $file
  echo
  echo "Radius: 10"
  mpirun -np 1 ./mpiblurc 10 $f $file
  echo
  echo "Radius: 100"
  mpirun -np 1 ./mpiblurc 100 $f $file
  echo
  echo "Processes: 16"
  echo "Radius: 5"
  mpirun -np 16 ./mpiblurc 5 $f $file
  echo
  echo "Radius: 10"
  mpirun -np 16 ./mpiblurc 10 $f $file
  echo
  echo "Radius: 100"
  mpirun -np 16 ./mpiblurc 100 $f $file
  echo
done
