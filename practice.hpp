#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <string_view>
#include <cassert>

namespace b3prac {

class StringSet {
public:
    static constexpr uint8_t kLeafChar = '\0';
    static constexpr uint8_t MaxUint8_t = 0xFF;
    static constexpr int kEmptyCheck = 0xFFFFFFFF;
    static constexpr int kEmptyBase = 0xFFFFFFFF;
    static constexpr int kFailedIndex = 0;
private:
    struct Unit {
        int base,check;
        Unit() : base(kEmptyBase), check(kEmptyCheck) {}
        Unit(int b, int c) : base(b),check(c) {}
    };
    std::vector<Unit> bc_;
    //bc_ = {kEmptyBase, 0}; // set root element
    

public:
    //StringSet() = default;
    StringSet() {
        bc_ = {{kEmptyBase, 0}}; // set root element
    }

    // 文字列を追加するための関数
    void insert(const std::string& str) {
        int node = 0;
        int n = 0;
        for(uint8_t c : str) {
            int next_node = Transition(node, c);
            std::unordered_map<uint8_t, int> tmp_base; // base値を保存する
            std::unordered_map<uint8_t, int> tmp_index; // indexを保存する
            std::vector<std::unordered_map<int, uint8_t>> tmp_child; // 子の子を保存する
            if (next_node == kFailedIndex) {
                auto row = GetChildren(node); // 子を取得
                std::cout << "----------------------------------------------------" << std::endl;
                std::cout << "row_size : " << row.size() << std::endl;
                std::cout << "node : " << node << std::endl;
                std::cout << "n : " << n << std::endl;
                uint8_t v = str[n];
                std::cout << "v: " << v << ", " << int(v) << std::endl;
                std::cout << "bc_size : " << bc_.size() << std::endl;
                std::cout << "next_node : " << next_node << std::endl;
                //std::cout << "row[0] : " << row[0] << std::endl;
                
                if (row.size() == 0) { // 子が存在しないとき
                    std::cout << "not child" << std::endl;
                    InsertSuffix(node, std::string_view(str).substr(n));
                    break;
                }
                else { // 子が存在するとき
                    int next_index = bc_[node].base + c;
                    if(bc_[next_index].check == kEmptyCheck) {
                        bc_[next_index].check = node;
                        std::cout << "children exists in space" << std::endl;
                        InsertSuffix(next_index, std::string_view(str).substr(n+1));
                        break;
                    }
                    else {
                        // いろいろな値の保存
                        for(int i=0; i < row.size(); i++) {
                            uint8_t a = row[i];
                            std::cout << "row_value : " << a << ", " << int(a) << std::endl;
                            int index = bc_[node].base + a;
                            tmp_base[a] = bc_[index].base;
                            tmp_index[a] = index;

                            // 子の子の値の保存
                            bool flag = false;
                            for(int i = 0; i < tmp_child.size(); i++) {
                                if(tmp_child[i][index]) {
                                    flag = true;
                                }
                            }
                            if(flag == false) {
                                for(int i=0; i < MaxUint8_t; i++) {
                                    uint8_t b = i;
                                    if(bc_[bc_[index].base + i].check == index) {
                                        tmp_child.emplace_back();
                                        tmp_child[tmp_child.size()-1][index] = i;
                                    }
                                }
                            }

                            // 使わない部部の消去
                            bc_[index].base = kEmptyBase;
                            bc_[index].check = kEmptyCheck;
                        }
                        std::cout << "children exists not space" << std::endl;
                        ModifyAndInsertSuffix(node, tmp_child, tmp_base, tmp_index, row, c, std::string_view(str).substr(n+1));
                        tmp_base.clear();
                        tmp_index.clear();
                        tmp_child.clear();
                        break;
                    }
                }
            }
            node = next_node;
            n++;
        }
    }

    bool contains(const std::string& key) const {
        int node = 0; // root
        //std::cout << "--------------contains---------------" << std::endl;
        //std::cout << "key : " << key << std::endl;
        for (uint8_t c : key) {
            //std::cout << "uint8_t : " << c << std::endl;
            //std::cout << "node  : " << node << std::endl;
            int next_node = Transition(node, c);
            //std::cout << "check : " << bc_[next_node].check << std::endl;
            if (next_node == kFailedIndex) {
                std::cout << "-------Fauiled value check------------" << std::endl;
                std::cout << key << std::endl;
                int tmp_base = 0;
                for(uint8_t a : key) {
                    std::cout << "c: " << a << ", " << int(a) << std::endl;
                    std::cout << "node : " << tmp_base << std::endl;
                    std::cout << "base : " << bc_[tmp_base].base << std::endl;
                    int tmp_next = bc_[tmp_base].base + a;
                    std::cout << "tmp_next : " << tmp_next << std::endl;
                    std::cout << "check : " << bc_[tmp_next].check << std::endl;
                    if(tmp_base != bc_[tmp_next].check) {
                        break;
                    }
                    tmp_base = tmp_next;
                }
                return false;
            }
            node = next_node;
        }
        // '\0'
        int next_node = Transition(node, kLeafChar);
        //std::cout << "--------kLeafer check---------------------" << std::endl;
        //std::cout << "key : " << key << std::endl;
        //std::cout << "node  : " << node << std::endl;
        //std::cout << "next_node : " << next_node << std::endl;
        //std::cout << "check : " << bc_[next_node].check << std::endl;
        return next_node != kFailedIndex;
    }

    void check() {
        std::cout << "------bc_size : " << bc_.size() << "---------" << std::endl;
        for(int i = 0; i < bc_.size(); i++) {
            //std::cout << "i : " << i << std::endl;
            std::cout << "check : " << bc_[i].check << std::endl;
        }
    }

private:
    int find_base(const std::unordered_map<uint8_t, int>& row) const {
        for (int base = 0; true; ++base) {
            bool found = true;
            for (auto p : row) {
                uint8_t c = p.first;
                int index = base + c;
                if (index < bc_.size() and bc_[index].check != kEmptyCheck) {
                    found = false;
                    break;
                }
            }
            if (found) {
                return base;
            }
        }
        return bc_.size();
    }

    void expand(int index) {
        if (index < bc_.size())
            return;
        bc_.resize(index+1);
    }

    // 残りの文字列を新たに格納
    void InsertSuffix(int r, std::string_view ax) {
        std::unordered_map<uint8_t, int> row;
        int node = r;
        for (int i =0; i < ax.size(); i++) {
            uint8_t c = ax[i];
            row[c] = 0;
            int base = find_base(row);
            bc_[node].base = base;
            int next_index = base + c;
            expand(next_index);
            bc_[next_index].check = node;
            node = next_index;
            row.clear();
        }
        // kLeafChar
        row[kLeafChar] = 0;
        int base = find_base(row);
        bc_[node].base = base;
        int next_index = base + kLeafChar;
        expand(next_index);
        bc_[next_index].check = node;
        row.clear();
    }

    // 更新
    void ModifyAndInsertSuffix(int r, 
        std::vector<std::unordered_map<int, uint8_t>> tmp_child,
        const std::unordered_map<uint8_t, int>& tmp_base, 
        const std::unordered_map<uint8_t, int>& tmp_index, 
        const std::vector<uint8_t>& row, 
        uint8_t c, 
        std::string_view str) 
        {
        int node = r;
        std::unordered_map<uint8_t, int> tmp_row;
        for(int i = 0; i < row.size(); i++) {
            tmp_row[row[i]] = 0;
        }
        tmp_row[c] = 0;
        int base = find_base(tmp_row);
        bc_[r].base = base;
        for (int i = 0; i < row.size(); i++) {
            uint8_t a = row[i];
            int next_index = base + a;
            expand(next_index);
            bc_[next_index].check = r;
            assert(tmp_base.count(a) == 1);
            bc_[next_index].base = tmp_base.find(a)->second;
            //std::cout << "tmp_base : " << bc_[next_index].base << std::endl;
            //子の子のcheck値を付け替える
            assert(tmp_index.count(a) == 1);
            int index = tmp_index.find(a)->second;
            //std::cout << "index : " << index << std::endl;
            if(a != kLeafChar) {
                //for(int i = 0; i < 26; i++) {
                    //uint8_t b = 'a' + i;
                //std::cout << "a : " << a << ", " << int(a) << std::endl;
                for(int j=0; j < tmp_child.size(); j++) {
                    if(tmp_child[j][index]) {
                        int next_next_index = bc_[next_index].base + tmp_child[j][index];
                        bc_[next_next_index].check = next_index;
                    }
                }
                /*
                for(int i = 0; i < MaxUint8_t; i++) {
                    uint8_t b = i;
                    int next_next_index = bc_[next_index].base + b;
                    if (bc_[next_next_index].check == index) {
                        std::cout << "b : " << b << ", " << int(b) << std::endl;
                        std::cout << "next_next_index : " << next_next_index << std::endl;
                        std::cout << "next_index : " << next_index << std::endl;
                        bc_[next_next_index].check = next_index;
                    }
                }
                */
            }
            if (a != kLeafChar) {
                int next_next_index = bc_[next_index].base + kLeafChar;
                if (bc_[next_next_index].check == index) {
                    bc_[next_next_index].check = next_index;
                }
            }
        }
        // uint8_t c の追加
        if (c != kLeafChar) {
            int next_index = base + c;
            expand(next_index);
            bc_[next_index].check = node;
            node = next_index;
            InsertSuffix(node, str);
        }
    }

    // 親番号に対する子の集合を返すための関数
    std::vector<uint8_t> GetChildren(int r) {
        std::vector<uint8_t> row;
        int count = 0;
        std::cout << "base : " << bc_[r].base << std::endl;
        if(bc_[r].base == kEmptyBase) {
            return row;
        }
        for(int i = 1; i < MaxUint8_t; i++) {
            uint8_t c = i;
            int idx = bc_[r].base + c;
            if(r == bc_[idx].check) {
                if (bc_.size() > idx) {
                    //row[c] = 0;
                    row.emplace_back();
                    row[count] = c;
                    count++;
                }
            }
        }

        if (r != 0) {
            int idx = bc_[r].base + kLeafChar;
            if(r == bc_[idx].check) {
                if (bc_.size() > idx) {
                    row.emplace_back();
                    row[kLeafChar] = 0;
                }
            }
        }

        return row;
    }

    // 遷移が成功するかどうかの判定(成功する場合は次の親番号を返値, 失敗した場合は0を返値)
    int Transition(int r, uint8_t c) const {
        auto base = bc_[r].base;
        
        if (base == kEmptyBase) {
            return kFailedIndex;
        }
        int t = base + c;
        return t < bc_.size() && bc_[t].check == r ? t : kFailedIndex;
    }

};

}