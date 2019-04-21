CXX = clang
SDL = -F /Library/Frameworks -framework SDL2 -framework SDL2_ttf
CXXFLAGS = -Wall -c
LDFLAGS = $(SDL)
# LDFLAGS = $(SDL) -lprofiler
EXE = dgchip8
TEST_EXE = test_dgchip8

# MAIN EXECUTABLE
################################################################################

all: $(EXE)

$(EXE): main.o global.o chip8.o instruction.o sdl_io.o
	$(CXX) $(LDFLAGS) $? -o $@

main.o: src/main.c src/global.h src/instruction.h
	$(CXX) $(CXXFLAGS) $< -o $@

global.o: src/global.c src/global.h
	$(CXX) $(CXXFLAGS) $< -o $@

chip8.o: src/chip8.c src/chip8.h
	$(CXX) $(CXXFLAGS) $< -o $@

instruction.o: src/instruction.c src/instruction.h
	$(CXX) $(CXXFLAGS) $< -o $@

sdl_io.o: src/sdl_io.c src/sdl_io.h
	$(CXX) $(CXXFLAGS) $< -o $@

# TEST
################################################################################

test: $(TEST_EXE)
	./$(TEST_EXE)

$(TEST_EXE): test_dgchip8.o instruction.o global.o
	$(CXX) $(LDFLAGS) $? -o $@
	# $(CXX) -o $(TEST_EXE) test_dgchip8.o instruction.o global.o $(LIBS)

test_dgchip8.o: tests/test_dgchip8.c tests/minunit.h
	$(CXX) $(CXXFLAGS) $< -o $@
	# $(CXX) $(CFLAGS) -c tests/test_dgchip8.c

# CLEANUP
################################################################################

clean:
	-rm $(TEST_EXE) $(EXE) *.o
