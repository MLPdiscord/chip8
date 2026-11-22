LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
LDFLAGS = -Ldependencies/raylib/lib
WARNINGS = -Wall -Wextra 
CPPFLAGS = $(WARNINGS) -Idependencies/raylib/include
SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=objects/%.o)
CXX = gcc

main: $(OBJECTS)
	$(CXX) -g $(LDFLAGS) -o $@ $^ $(LDLIBS)

objects/main.o: src/main.c
	$(CXX) -g $(CPPFLAGS) -c $< -o $@

objects/%.o: src/%.c src/%.h
	$(CXX) -g $(CPPFLAGS) -c $< -o $@


clean:
	rm -rf objects
	mkdir objects