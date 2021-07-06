TARGET = opencv_test
OBJS   = opencv_test.cpp v4l2_camera.c 
OBJS   += ./base/base64.c
OBJS   += ./base/file_ops.c
CC  		= aarch64-linux-gnu-g++
LIBS    	= -L /usr/lib/aarch64-linux-gnu/
CPPFLAGS    	= -lopencv_core -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lpthread -lrt -g
LINC     	+= -I /usr/include/opencv2/

$(TARGET):$(OBJS)
	$(CC) -std=c++11 $^  -o $@ $(LINC) $(LIBS) $(CPPFLAGS) 

clean:
	$(RM) *.o $(TARGET) -r
