MAKEFLAGS += -L

.PHONY: clean
CXX = clang

thief: main.c netlib.c parser.c ./lib/elib.c
	$(CXX) $^ -o $@

main.o: main.c thief.h
	$(CXX) -c $@

parser.o: parser.c ./lib/elib.c 
	$(CXX) -c $@ 

netlib.o: netlib.c ./lib/elib.c 
	$(CXX) -c $@ 

elib.o: ./lib/elib.c 
	$(CXX) -c $@

clean: 
	-rm *.o *.txt
