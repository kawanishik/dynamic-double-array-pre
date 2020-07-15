#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>

#include "practice.hpp"

namespace b3prac {

class Tester {
private:
    int num;
    std::unordered_map<std::string, bool> samples;
    b3prac::StringSet strset;
public:
    Tester() = default;
    Tester(int num) : num(num) {
        std::string input_name = std::string("s")+char(num+'0')+".txt";
        std::ifstream ifs(input_name);
        if (!ifs) {
            std::cerr<<"File not found input file: "<<input_name<<std::endl;
            exit(0);
        }
        std::vector<std::string> str_list;
        for (std::string s; std::getline(ifs, s); ) {
            str_list.push_back(s);
        }
        //for (auto s:str_list)
        //strset = b3prac::StringSet(str_list);

        
        int n = 0;
        for(std::string str : str_list) {
            bool check = true;
            std::cout << str_list[n] << std::endl;
            n++;
            int n2;
            strset.insert(str);
            for(int i=0; i < n; i++) {
                check &= test(str_list[i]);
                if(!check) {
                    for(int j = i+1; j < n; j++) {
                        test(str_list[j]);
                    }
                    break;
                }
            }
            std::cout << (check ? "OK" : "Failed") << std::endl;
            if(!check) {
                exit(1);
            }
        }
        //strset.check();
    }
    bool test(const std::string& str) {
        return(strset.contains(str));
    }
};
    
}