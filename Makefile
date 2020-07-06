gcc_options = --std=c++17 -Wall
debug_options = -g

test_da : test.cpp
	g++ $(gcc_options) test.cpp -o test_da

debug : test.cpp
	g++ $(gcc_options) $(debug_options) test.cpp -o test_da

clean : 
	rm -f test_da

.PHONY : test_da clean
