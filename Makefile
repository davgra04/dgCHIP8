CFLAGS=-Wall
# LIBS=-lcheck
SRC_DIR=src
TESTS_DIR=tests

all: dgchip8

dgchip8: main.o global.o instruction.o
	gcc -o dgchip8 main.o global.o instruction.o

main.o: $(SRC_DIR)/main.c $(SRC_DIR)/global.h $(SRC_DIR)/instruction.h
	gcc $(CFLAGS) -c $(SRC_DIR)/main.c

global.o: $(SRC_DIR)/global.c $(SRC_DIR)/global.h
	gcc $(CFLAGS) -c $(SRC_DIR)/global.c

instruction.o: $(SRC_DIR)/instruction.c $(SRC_DIR)/instruction.h
	gcc $(CFLAGS) -c $(SRC_DIR)/instruction.c


test: test_dgchip8
	./test_dgchip8

test_dgchip8: test_dgchip8.o instruction.o global.o
	gcc -o test_dgchip8 test_dgchip8.o instruction.o global.o $(LIBS)

test_dgchip8.o: $(TESTS_DIR)/minunit.h $(TESTS_DIR)/test_dgchip8.c
	gcc $(CFLAGS) -c $(TESTS_DIR)/test_dgchip8.c

clean:
	-rm test_dgchip8 dgchip8 *.o

# all:
# 	gcc src/$(SRC_DIR)/main.c -o dgchip8
