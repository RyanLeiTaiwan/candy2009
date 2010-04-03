CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		windows.o

LIBS =

TARGET =	windows.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
