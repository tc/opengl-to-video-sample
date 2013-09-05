CC=g++

CFLAGS+=`pkg-config --cflags opencv`
LDFLAGS+=`pkg-config --libs opencv`

PROG=hello
OBJS=$(PROG).o util.o

GLEW_LIB = /opt/local/lib

FRAMEWORKS = -framework GLUT -framework OpenGL

LIBS_ffmpeg = -lm -lz -lpthread -lavformat -lavcodec -lavutil

LIBS_opencv = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_objdetect

LIBS_others = -lpthread -ldl -lm -lGLEW -ljpeg

LIBS = $(LIBS_others) $(LIBS_ffmpeg) $(LIBS_opencv)

.PHONY: all clean

$(PROG): $(OBJS)
	$(CC) -o $(PROG) $(FRAMEWORKS) $(OBJS) $(LIBS) $(LDFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) $<

all: $(PROG)

clean:
	rm -f $(OBJS) $(PROG)

