#include "tester.hpp"

#include <iostream>

namespace {
/*
void test(int num) {
    if (b3prac::Tester(num).test()) {
        std::cout << "...OK" << std::endl;
    }
    else {
        std::cout << "...Failed" << std::endl;
    }
}
*/

}

int main(int argc, char* argv[]) {
    b3prac::Tester tester;
    if (argc == 1) {
        for (int i = 0; i < 5; i++) {
            std::cout << "Test" << i << std::endl;
            //test(i);
        }
    } else {
        //test(argv[1][0]-'0');
        tester = b3prac::Tester(argv[1][0]-'0');
        /*
        if(tester.test()) {
            std::cout << "...OK" << std::endl;
        }
        else {
            std::cout << "...Failed" << std::endl;
        }
        */
    }
}