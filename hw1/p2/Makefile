CFLAGS = -g -std=c++11  -Wall

p2Run: p2Main.o p2Table.o
	g++ -o $@ p2Main.o p2Table.o $(CFLAGS)

p2Main.o: p2Main.cpp p2Table.h
	g++ -c p2Main.cpp $(CFLAGS)

p2Table.o: p2Table.cpp p2Table.h
	g++ -c p2Table.cpp $(CFLAGS)

clean:
	rm -f p2Run *.o
