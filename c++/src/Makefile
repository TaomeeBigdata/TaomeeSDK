#----------------------------------
# example Makefile used to generate
# binary .so file
#----------------------------------

XX		= g++
AR		= ar
INCLUDES	= -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I../
# 实测O3情况下，性能有20%的提升
#FLAGS		=  -Wall -DDEBUG -g -fPIC -MMD
FLAGS		= -std=c++0x -Wall -DDEBUG -g -shared -fPIC -MMD
SOURCE		= $(wildcard *.cpp)
OBJS		= $(patsubst %.cpp, %.o, $(SOURCE))
TARGET		= ./libstatlogger64bit.a

all: $(TARGET)
%.o: %.cpp Makefile
	$(XX) $(FLAGS) $(INCLUDES) -c $< -o $@
	@-mv -f $*.d .dep.$@

$(TARGET): $(OBJS)
	$(AR) rcs $@ $^
	rm -f *.o


clean:
	rm -rf *.o .dep.* $(TARGET)

#-include /dev/null $(wildcard .dep.*)
