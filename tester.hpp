#include <unordered_map>
#include <string>
#include <fstream>
#include <iostream>

#include "practice.hpp"

namespace b3prac {

class Tester {
private:
    int num;
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
            strset.insert(str);
            for(int i=0; i< n; i++) {
                check &= test(str_list[i]);
                if(!check) {
                    break;
                }
            }
            //strset.SizeCheck();
            std::cout << (check ? "OK" : "Failed") << std::endl;
            //std::cout << std::endl;
            if(!check) {
                //strset.CheckContent();
                //strset.CheckTAIL();
                exit(1);
            }
            /*
            std::cout << "str_size : " << str.size() << std::endl;
            for(int i=0; i < str.size(); i++) {
                uint8_t c = str[i];
                std::cout << "i : " << i << ", " << c << ", " << int(c) << std::endl;
            }
            */
        }
        //strset.CheckContent();
        //strset.SizeCheck();
        //strset.CheckTAIL();
        strset.EmptyNum();
    }
    bool test(const std::string& str) {
        return(strset.contains(str));
    }
};
    
}