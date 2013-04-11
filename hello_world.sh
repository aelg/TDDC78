#!/bin/bash

#SBATCH -J hello_world
#SBATCH -t 00:00:10
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH --reservation=devel
#
mpprun ./hello_world

# Script ends here
