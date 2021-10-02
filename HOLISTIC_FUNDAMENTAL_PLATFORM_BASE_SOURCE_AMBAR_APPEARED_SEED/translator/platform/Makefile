CXXFLAGS = -std=c++0x -O0 -g -Wall -fmessage-length=0

OBJS =		HelloWorld.o

LIBS =

TARGET =	HelloWorld

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
