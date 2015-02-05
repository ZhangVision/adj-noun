all : annotation.cpp
	g++ annotation.cpp -o annotation
	
clean:
	rm -rf annotaion
