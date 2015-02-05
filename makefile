all : annotation.cpp
	g++ -Wall -g annotation.cpp -o annotation
	
clean:
	rm -rf annotaion
