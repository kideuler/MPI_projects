CFLAGS = -I -Isrc -Iinclude
DEPS = src/*.cpp
DEPS_O = *.o
OBJ = main
ADDITIONAL_FLAGS = 
RUNFLAGS =
OBJDIR = bin

ifneq ($(np), 1)
	CC = mpic++
	ADDITIONAL_FLAGS += -D USE_MPI
	RUNFLAGS += mpirun -np $(np)
else
	CC = g++
endif

make:
	$(CC) $(CFLAGS) $(ADDITIONAL_FLAGS) -c -O3 $(DEPS)
	$(CC) $(CFLAGS) $(ADDITIONAL_FLAGS)  -g main.cpp $(DEPS_O) -o $(OBJ)
	$(RUNFLAGS) ./$(OBJ)
.PHONY: clean
clean:
	rm -rf *.o *.vtk
.PHONY: debug
debug:
	$(CC) $(CFLAGS) $(ADDITIONAL_FLAGS) -c -g $(DEPS)

multi:
	mpic++ $(CFLAGS) -c -g $(DEPS)


.PHONY: run
run:
	$(CC) $(CFLAGS)  -g main.cpp $(DEPS_O) -o $(OBJ)
	$(RUNFLAGS) ./$(OBJ)