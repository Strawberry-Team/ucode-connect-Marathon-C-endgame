FRAMEWORK_LIB = resources/framework/raylib/src/libraylib.a
NAME = game
SRC = src/*.c


.PHONY: all reinstall

all:
	clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL $(FRAMEWORK_LIB) $(SRC) -o $(NAME)

clean:
	rm $(NAME)

reinstall: clean all