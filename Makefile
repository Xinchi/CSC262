# This makefile is intended for the GNU C compiler.
# Your code must compile (with GCC) with the given CFLAGS.
# You may experiment with the OPT variable to invoke additional compiler options
HOST = $(shell hostname)
BANG   =  $(shell expr match `hostname` ccom-bang)
BANG-COMPUTE   =  $(shell expr match `hostname` compute)
CARVER =  $(shell expr match `hostname` cvrsvc)


ifneq ($(BANG), 0)
atlas := 1
include $(PUB)/Arch/arch.gnu-4.7_c99.generic
else
ifneq ($(BANG-COMPUTE), 0)
atlas := 1
include $(PUB)/Arch/arch.gnu-4.7_c99.generic
else
NO_BLAS = 0
ifneq ($(CARVER), 0)
PUB = /global/u2/b/baden/cse260-wi14
C++FLAGS += -DUSE_MKL
CFLAGS += -DUSE_MKL
CFLAGS += -std=c99 
C++FLAGS += -std=c99 
include $(PUB)/Arch/arch.intel-mkl
else
PUB = /Path/To/Your/Own/machine/here
include $(PUB)/Arch/arch.gnu
endif
endif
endif

WARNINGS += -Wall -pedantic

#
# If you want to add symbol table information for gdb/cachegrind
# specify dbg=1 on the "make" command line
ifeq ($(dbg), 1)
        CFLAGS += -g
        LDFLAGS += -g
        C++FLAGS += -g
endif



# Compiler for gprof
ifeq ($(gprof), 1)
        CFLAGS += -g -pg
        C++FLAGS += -g -pg
        LDFLAGS += -g -pg
endif

# If you want to copy data blocks to contiguous storage
# This applies to the hand code version
ifeq ($(copy), 1)
    C++FLAGS += -DCOPY
    CFLAGS += -DCOPY
endif


# If you want to use restrict pointers, make restrict=1
# This applies to the hand code version
ifeq ($(restrict), 1)
    C++FLAGS += -D__RESTRICT
    CFLAGS += -D__RESTRICT
# ifneq ($(CARVER), 0)
#    C++FLAGS += -restrict
#     CFLAGS += -restrict
# endif
endif

ifeq ($(NO_BLAS), 1)
    C++FLAGS += -DNO_BLAS
    CFLAGS += -DNO_BLAS
endif


#DEBUG += -DDEBUG



targets = benchmark-naive benchmark-blocked benchmark-blas
objects = benchmark.o dgemm-naive.o dgemm-blocked.o dgemm-blas.o  
UTIL   = wall_time.o cmdLine.o

.PHONY : default
default : all

.PHONY : all
all : clean $(targets)

benchmark-naive : benchmark.o dgemm-naive.o  $(UTIL)
	$(CC) -o $@ $^ $(LDLIBS)
benchmark-blocked : benchmark.o dgemm-blocked.o $(UTIL)
	$(CC) -o $@ $^ $(LDLIBS)
benchmark-blas : benchmark.o dgemm-blas.o $(UTIL)
	$(CC) -o $@ $^ $(LDLIBS)

%.o : %.c
	$(CC) -c $(CFLAGS) $<


.PHONY : clean
clean:
	rm -f $(targets) $(objects) $(UTIL) core
