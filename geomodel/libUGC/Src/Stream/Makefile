uname_m = $(shell uname -m)

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

TARGET = libYkStream.so 

LD_FLAGS.x86_64 = ../../Builds/MakeProject/Bin64_Linux
LD_FLAGS.aarch64 = ../../Builds/MakeProject/Binaarch64_Linux

CFLAGS.x86_64 = -m64
CFLAGS.aarch64 =


CFLAGS = -I../../Include -Wall -DSYSTEM_WCHAR_SIZE=4 -D_UGUNICODE -D_UGQSTRING -D_UGLICENSE -fshort-wchar \
		 -DSTATIC_LIBMONGOCLIENT -fstack-protector-all -fPIE -pie -fPIC -c  -fopenmp -std=c++11  \
		 -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DNDEBUG -D_FORTIFY_SOURCE=2  $(CFLAGS.$(uname_m))

LD_FLAGS += $(LD_FLAGS.$(uname_m)) 

%.o:%.cpp
	g++ $(CFLAGS) $(IS_DEBUG) -c $^ -o $@

$(TARGET) : $(objects)
	g++ -L$(LD_FLAGS) $(IS_DEBUG) $(CFLAGS.$(uname_m)) -Wl,-soname,$(TARGET) -Wl,--hash-style=sysv  -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack \
	-shared -rdynamic -lz -lpthread -lrt -lQt5Core -lstdc++ -o $@ $(objects) -lYkBase -lQt5Core
	cp $(TARGET) $(LD_FLAGS)

clean:
	rm -rf *.o
	rm -rf *.so


	
