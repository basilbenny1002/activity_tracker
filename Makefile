CC = gcc
CFLAGS = -Wall -lpsapi  

TARGET = activity_tracker

SRC = activity_tracker.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	del $(TARGET).exe
