# Danju Visvanathan
# COMP3520 Assignment 1
# 312095252
# 2015 Semester 1
CC = gcc
CFLAGS = -g -Wall -O0
TARGET=myshell
myshell: $(TARGET).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c
$(TARGET).o: $(TARGET).c
clean:
	rm -f $(TARGET) $(TARGET).o

