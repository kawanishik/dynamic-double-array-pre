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
        auto time_point = std::chrono::high_resolution_clock::now();
        for(std::string str : str_list) {

            bool check = true;
            //std::cout << str_list[n] << std::endl;
            n++;
            strset.insert(str);
            /*
            if(n % 1000 == 0) {
                //std::cout << strset.EmptyRate() << std::endl;
                auto rate = strset.EmptyRate();
                std::cout << rate << std::endl;
                if(rate < 0.8)
                    strset.REBUILD();
            }
            */
            /*
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
            */
            /*
            std::cout << "str_size : " << str.size() << std::endl;
            for(int i=0; i < str.size(); i++) {
                uint8_t c = str[i];
                std::cout << "i : " << i << ", " << c << ", " << int(c) << std::endl;
            }
            */
        }
        auto duration = std::chrono::high_resolution_clock::now() - time_point;
        double count = std::chrono::duration<double, std::milli>(duration).count();
        printf("構築時間 : %.1lf\n", count);
        // 検索時間を調べる
        time_point = std::chrono::high_resolution_clock::now();
        bool check = true;
        for(int i=0; i< n; i++) {
            check = test(str_list[i]);
            if(!check) {
                break;
            }
        }
        duration = std::chrono::high_resolution_clock::now() - time_point;
        count = std::chrono::duration<double, std::micro>(duration).count();
        //std::cout << "処理時間:" << count << "[μs]" << std::endl;
        //double count2 = count;
        //count /= n;
        std::cout << (check ? "OK" : "Failed") << std::endl;
        printf("平均検索時間 : %.4f\n", count / n);
        strset.EmptyNum();
        strset.CheckTAIL();

        // REBUILD
        std::cout << std::endl;
        time_point = std::chrono::high_resolution_clock::now();
        strset.REBUILD();
        duration = std::chrono::high_resolution_clock::now() - time_point;
        count = std::chrono::duration<double, std::milli>(duration).count();
        printf("REBUILD時間 : %.1lf\n", count);
        //bool check = true;
        time_point = std::chrono::high_resolution_clock::now();
        check = true;
        for(int i=0; i< n; i++) {
            check = test(str_list[i]);
            if(!check) {
                break;
            }
        }
        duration = std::chrono::high_resolution_clock::now() - time_point;
        count = std::chrono::duration<double, std::micro>(duration).count();
        //count /= n;
        std::cout << (check ? "OK" : "Failed") << std::endl;
        printf("平均検索時間 : %.4f\n", count / n);
        //std::cout << (check ? "OK" : "Failed") << std::endl;
        strset.CheckTAIL();
        strset.EmptyNum();
    }
    bool test(const std::string& str) {
        return(strset.contains(str));
    }
};
    
}