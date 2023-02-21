FRAMEWORK_LIB = resources/framework/raylib/src/libraylib.a
NAME = endgame
SRC = src/*.c


.PHONY: macos linux reinstall

macos:
	clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL $(FRAMEWORK_LIB) $(SRC) -o $(NAME)

linux:
	clang gcc $(SRC) -o $(NAME) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

clean:
	rm $(NAME)

reinstall: clean