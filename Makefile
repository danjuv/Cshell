# Danju Visvanathan
# COMP3520 Assignment 1
# 312095252
# 2015 Semester 1
# to compile: make myshell or make
# to clean: make clean
CC = gcc
CFLAGS = -g -O0
TARGET=myshell
myshell: $(TARGET).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c
$(TARGET).o: $(TARGET).c
clean:
	rm -f $(TARGET) $(TARGET).o

