CFLAGS = -std=c++11
FLAGS_DEBUG = -g -O0
FLAGS_LLVM = $(shell llvm-config --cxxflags)
FLAGS_FLTK = $(shell fltk-config --use-images --cxxflags )
#LDFLAGS  = $(shell fltk-config --use-images --ldflags )
LD_FLTK = $(shell fltk-config --use-images --ldstaticflags)
LINK = $(CXX)

# this is a pattern rule
#.SUFFIXES: .o .cxx
#%.o: %.cxx
#	$(CXX) $(CXXFLAGS) $(DEBUG) -c $<

all: mpw

mpw: mpw.cxx Makefile
	g++ $(CFLAGS) $(FLAGS_FLTK) $(FLAGS_DEBUG) $(LD_FLTK) mpw.cxx base64.cxx -o mpw

# OTHER targets
#
clean: $(TARGET) $(OBJS)
	rm -f *.o 2> /dev/null
	rm -f $(TARGET) 2> /dev/null
	rm e 2> /dev/null

install:
	install ./mpw /usr/local/bin

symlink:
	ln -s $(PWD)/mpw /usr/local/bin/mpw 

