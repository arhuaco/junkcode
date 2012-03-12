OPTFLAGS=-DDEBUG -mtune=pentium4 -mcpu=pentium4 -I. -ffast-math  -fno-schedule-insns -fno-loop-optimize


seesound.so: seesound_wrap.cxx seesound.cpp seesound.h seesound.i
	g++ -c -fPIC `python-config  --cflags` ${OPTFLAGS} seesound.cpp seesound_wrap.cxx  -I.
	g++ -shared seesound_wrap.o seesound.o -o _seesound.so

seesound_wrap.cxx: seesound.h seesound.i seesound.cpp
	swig -classic -c++ -python seesound.i

.PHONY: clean

clean:
	rm -rf seesound_wrap.cxx _seesound.so  seesound.py seesound.o  _seesound.so  seesound_wrap.o

