#default build suggestion of MPI + OPENMP with gcc on Livermore machines you might have to change the compiler name

SHELL = /bin/sh
.SUFFIXES: .cc .o .ll

ENZYME_PATH ?= /home/ubuntu/enzyme-sc22/Enzyme/enzyme/build/Enzyme/ClangEnzyme-15.so
CLANG_PATH ?= /home/ubuntu/enzyme-sc22/llvm-project/build/bin
#ENZYME_PATH ?= /home/wmoses/git/Enzyme/enzyme/build13D/Enzyme/ClangEnzyme-13.so
#CLANG_PATH ?= /mnt/sabrent/wmoses/llvm13/buildall/bin

OPENMP_PATH ?= $(CLANG_PATH)/../projects/openmp/runtime/src
MPI_PATH ?= /usr/lib/x86_64-linux-gnu/openmpi/include
OPENMP_LIB ?= $(CLANG_PATH)/../lib/libomp.so

MPI_INC = /opt/local/include/openmpi
MPI_LIB = /opt/local/lib

CXX = $(CLANG_PATH)/clang++ -DUSE_MPI=1

LULESH_EXEC = lulesh2.0

TYPEART_RTPATH = ${TYPEART_PATH}/lib
LTYPEART_RT= -L$(TYPEART_RTPATH) -ltypeart-rt 

MEDIPACK_DIR=../medi
CODIPACK_DIR=../codi

SOURCES2.0 = \
	lulesh.cc \
	lulesh-comm.cc \
	lulesh-viz.cc \
	lulesh-util.cc \
	lulesh-init.cc \
  lulesh-ad.cc
OBJECTS2.0 = $(SOURCES2.0:.cc=.o)

#Below are reasonable default flags for a serial build w/o OpenMP
CXXFLAGS = -g -I. -Wall -Wno-format-security -Wno-overloaded-virtual -I${MEDIPACK_DIR}/include -I${MEDIPACK_DIR}/src -I${CODIPACK_DIR}/include -I $(OPENMP_PATH) -I $(MPI_PATH) 
LDFLAGS = -g -O3

PASSES=-load ${TYPEART_PATH}/lib/meminstfinderpass.so -load ${TYPEART_PATH}/lib/typeartpass.so
CALL_FILTER=-call-filter -call-filter-deep

.cc.o: lulesh.h
ifeq ($(TYPEART), yes)
	$(CXX) ${CXXFLAGS} -O1 -Xclang -disable-llvm-passes -S -emit-llvm $< -o - | opt $(PASSES) -typeart -S | opt -O3 -S | opt $(PASSES) -typeart -typeart-alloca -alloca-array-only=false $(CALL_FILTER) -typeart-no-heap=true | llc -x=ir -filetype=obj -o $@
else
	$(CXX) $(CXXFLAGS) -O3 -c -o $@ $<
endif

all: $(LULESH_EXEC)

lulesh2.0: $(OBJECTS2.0)
	@echo "Linking" 
ifeq ($(TYPEART), yes)
	$(CXX) $(OBJECTS2.0) $(LDFLAGS) -lm $(LTYPEART_RT) -o $@ -lmpi
else
	$(CXX) $(OBJECTS2.0) $(LDFLAGS) -lm -o $@ -lmpi
endif

medi: medi-main.cpp
	$(CXX) $(CXXFLAGS) -O0 -o $@ $<

clean:
	/bin/rm -f *.o *.s *~ $(OBJECTS) $(LULESH_EXEC) types.yaml
	/bin/rm -rf *.dSYM

tar: clean
	cd .. ; tar cvf lulesh-2.0.tar LULESH-2.0 ; mv lulesh-2.0.tar LULESH-2.0
