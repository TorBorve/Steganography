steganography: main.o steg.o pngUtilities.o
	g++ main.o steg.o pngUtilities.o -o steganography -lpng

main.o: main.cpp
	g++ -c main.cpp

steg.o: steg.cpp steg.h
	g++ -c steg.cpp

pngUtilities.o: pngUtilities.cpp pngUtilities.h
	g++ -c pngUtilities.cpp

clean:
	rm *.o steganography