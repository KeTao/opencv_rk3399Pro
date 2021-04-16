TARGET = opencv_test
OBJS   = opencv_test.cpp camera_test.c
CC  		= aarch64-linux-gnu-g++
LIBS    	= -L /usr/lib/aarch64-linux-gnu/
CPPFLAGS    	= -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lpthread -lrt -g
LINC     	+= -I /usr/include/opencv2/

$(TARGET):$(OBJS)
	$(CC) $^ -o $@ $(LINC) $(LIBS) $(CPPFLAGS) 

clean:
	$(RM) *.o $(TARGET) -r
