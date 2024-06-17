CC = g++
# simplest compilation, do not depend on external libraries
# -DNO_FFT: you cannot Fourier-transform autocorrelation functions directly inside the program (but this is more convenient to do outside the program anyway)
# -DNO_BOOST: you cannot do polyhedron identification/classification (requires spherical harmonics) - but this is so experimental/poorly documented that you probably don't want to do it anyway
# -DSVN_REV: a version number, relic from when svn was used
CFLAGS = -O2 -g -fopenmp -I ./include  -D_USE_MATH_DEFINES  -DNO_FFT -DNO_BOOST -DSVN_REV='"0.5"'
COMPILE = $(CC) $(CFLAGS) -c -std=c++0x  
OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
OBJFILES := $(filter-out mbd.cpp, $(OBJFILES))

all: lionanalysis

lionanalysis: $(OBJFILES)
	$(CC) -fopenmp -o lionanalysis.exe $(OBJFILES) -lm
	#cp lionanalysis.exe ~/bin/lionanalysis.exe

%.o: %.cpp
	$(COMPILE) -o $@ $<

clean:
	rm *.o
