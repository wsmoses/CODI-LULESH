#!/usr/bin/python3.8
import os
import pathlib
scriptdir = pathlib.Path(__file__).parent.resolve()

def printfun(rank, blocklist,itercount):
  for s in blocklist:
      for mode in ["forward","gradient"]:
        os.system("mpirun -n {} taskset -c 0-{} numactl -i all ".format(rank,rank-1)+ str(scriptdir) + "/../../lulesh-{}/lulesh2.0 -s {} -i {} > ".format(mode,s,itercount) + str(scriptdir) + "/ser-mpi_{}_{}_{}_{}.txt".format(mode,rank,itercount,s))

itercount=100
printfun(1, [48],itercount)
printfun(8, [48],itercount)
printfun(27, [48],itercount)
printfun(64, [48],itercount)
