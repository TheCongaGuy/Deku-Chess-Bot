# Default Configuration
default: DekuBot.hpp GameBoard.hpp Sprite.h Test.hpp
	g++ -c main.cpp gameBoard.cpp test.cpp dekuBot.cpp
	g++ main.o gameBoard.o test.o dekuBot.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
	./sfml-app