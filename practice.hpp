#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <string_view>
#include <cassert>
#include <time.h>

namespace b3prac {

class StringSet {
public:
    static constexpr uint8_t kLeafChar = 0;
    static constexpr uint8_t MaxUint8_t = 0xFF;
    static constexpr int kEmptyCheck = 0xFFFFFFFF;
    static constexpr int kEmptyBase = 0xFFFFFFFF;
    static constexpr int kFailedIndex = 0;
private:
    struct Unit {
        int base,check;
        bool is_used;
        Unit() : base(kEmptyBase), check(kEmptyCheck), is_used(true) {}
        Unit(int b, int c, bool f) : base(b),check(c),is_used(f) {}
    };
    std::vector<Unit> bc_;
    int E_HEAD = -1;
    //bc_ = {kEmptyBase, 0}; // set root element
    

public:
    //StringSet() = default;
    StringSet() {
        bc_ = {{kEmptyBase, 0, false}}; // set root element
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
                //std::cout << "----------------------------------------------------" << std::endl;
                //std::cout << "row_size : " << row.size() << std::endl;
                //std::cout << "node : " << node << std::endl;
                //std::cout << "n : " << n << std::endl;
                uint8_t v = str[n];
                //std::cout << "v: " << v << ", " << int(v) << std::endl;
                //std::cout << "bc_size : " << bc_.size() << std::endl;
                //std::cout << "next_node : " << next_node << std::endl;
                
                if (row.size() == 0) { // 子が存在しないとき
                    //std::cout << "not child" << std::endl;
                    InsertSuffix(node, std::string_view(str).substr(n));
                    break;
                }
                else { // 子が存在するとき
                    int next_index = bc_[node].base + c;
                    expand(next_index);
                    if(bc_[next_index].is_used == true) {
                        AddCheck(next_index, node);
                        //bc_[next_index].check = node;
                        //bc_[next_index].is_used = false;
                        //std::cout << "children exists in space" << std::endl;
                        InsertSuffix(next_index, std::string_view(str).substr(n+1));
                        break;
                    }
                    else {
                        // いろいろな値の保存
                        for(int i=0; i < row.size(); i++) {
                            uint8_t a = row[i];
                            int index = bc_[node].base + a;
                            tmp_base[a] = bc_[index].base;
                            tmp_index[a] = index;

                            // 子の子の値の保存
                            bool is_child = false;
                            for(int i = 0; i < tmp_child.size(); i++) {
                                if(tmp_child[i][index]) {
                                    is_child = true;
                                }
                            }
                            if(is_child == false) {
                                for(int i=0; i < MaxUint8_t; i++) {
                                    uint8_t b = i;
                                    if(bc_.size() < (bc_[index].base + b)) {
                                        break;
                                    }
                                    if(bc_[bc_[index].base + b].check == index) {
                                        tmp_child.emplace_back();
                                        tmp_child[tmp_child.size()-1][index] = b;
                                    }
                                }
                            }

                            // 使わない部部の消去
                            Delete(index);
                        }
                        //std::cout << "children exists not space" << std::endl;
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
        //std::cout << "size : " << key.size() << std::endl;
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
                    std::cout << "flag : " << bc_[tmp_base].is_used << std::endl;
                    int tmp_next = bc_[tmp_base].base + a;
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
        //std::cout << "node  : " << node << std::endl;
        //std::cout << "check : " << bc_[next_node].check << std::endl;
        return next_node != kFailedIndex;
    }

    void SizeCheck() {
        std::cout << "bc_size : " << bc_.size() << std::endl;
    }

    void CheckContent() {
        std::cout << "----- CheckContent -----" << std::endl;
        for(int i=0; i < bc_.size(); i++) {
            if(bc_[i].is_used == true) {
                std::cout << "check : " << bc_[i].check << ", i : " << i << std::endl;
            }
        }
        std::cout << "-------------base--------------" << std::endl;
        for(int i=0; i < bc_.size(); i++) {
            if(bc_[i].is_used == true) {
                std::cout << "base : " << bc_[i].base << ", i : " << i << std::endl;
            }
        }
    }

private:
    int find_base(const std::unordered_map<uint8_t, int>& row) const {
        //std::cout << "--------------find_base----------------" << std::endl;
        int e_index = E_HEAD;
        int base;
        int roop = 0;
        /*
        if(bc_.size() == 1) {
            return bc_.size();
        }
        */
        if(E_HEAD == -1) {
            return bc_.size();
        }

        // row の中で，一番小さい数字を選択
        uint8_t c = MaxUint8_t;
        if(row.size() == 0) {
            c = kLeafChar;
        }
        for(auto p : row) {
            uint8_t a = p.first;
            if(c > a) {
                c = a;
            }
        }
        if(bc_[e_index].is_used == false) {
            return bc_.size();
        }
        //std::cout << "process find base" << std::endl;
        while(true) {
            //std::cout << "--------while------" << std::endl;
            //std::cout << "bc_size : " << bc_.size() << std::endl;
            //std::cout << "e_index : " << e_index << std::endl;
            //std::cout << "e_index-is_used : " << bc_[e_index].is_used << std::endl;
            //std::cout << "e_index-base : " << bc_[e_index].check << std::endl;

            roop += 1;
            bool found = true;
            base = e_index - c;
            
            if(base > 0) {
                for(auto p : row) {
                    uint8_t b = p.first;
                    int next_row = p.second;
                    if((base + b) >= bc_.size()) {
                        continue;
                    }
                    if(bc_[base + b].is_used == false) { // falseは使われているという意味
                        e_index = bc_[e_index].check;
                        if(e_index == E_HEAD) {
                        //if(e_index == bc_.size()) {
                            std::cout << "E_HEAD match" << std::endl;
                            return bc_.size();
                        }
                        found = false;
                        break;
                    }
                }
            }
            else {
                found = false;
                e_index = bc_[e_index].check;
                if(e_index == E_HEAD) {
                    return bc_.size();
                }
            }
            if(found) {
                return base;
            }
        }
        return bc_.size();
    }

    void expand(int index) {
        //std::cout << "----------------expand---------------" << std::endl;
        //std::cout << "pre_size : " << bc_.size() << std::endl;
        if (index < bc_.size())
            return;
        
        int size_pre = bc_.size();
        //std::cout << "flag : " << bc_[E_HEAD].is_used << std::endl;
        bc_.resize(index+1);
        int size = bc_.size();
        
        //std::cout << "size : " << size << std::endl;
        if(size_pre == size) {
            return;
        }
        
        // E_HEADのbaseには，末尾のindexが格納されている
        if(E_HEAD == -1) {
            E_HEAD = size_pre;
            for(int i = size_pre; i < size; i++) {
                if(i > size_pre) {
                    bc_[i].base = -1 * (i-1);
                    //std::cout << "bc_[i].base : " << bc_[i].base << std::endl;
                }
                bc_[i].check = i+1;
                bc_[i].is_used = true;
            }
            bc_[size-1].check = size_pre;
            bc_[size_pre].base = -1 * (size-1);
        }
        else {
            int e_index = E_HEAD;

            if(bc_[e_index].is_used == false) {
                //std::cout << "------------------------- flag false -----------------------" << std::endl;
                //std::cout << "e_index : " << e_index << std::endl;
                //std::cout << "flag : " << bc_[e_index].is_used << std::endl;
                //std::cout << "size_pre : " << size_pre << std::endl;
                //std::cout << "size : " << size << std::endl;
                for(int i=size_pre; i < size; i++) {
                    if(i > size_pre) {
                        bc_[i].base = -1 * (i-1);
                    }
                    bc_[i].check = i+1;
                    bc_[i].is_used = true;
                }
                E_HEAD = size_pre;
                bc_[size-1].check = size_pre;
                bc_[size_pre].base = -1 * (size - 1);
            }
            else {
                int f_index = -1 * bc_[e_index].base; // サイズを拡張する前の最後のindex
                //std::cout << "e_index : " << e_index << std::endl;
                //std::cout << "f_index : " << f_index << std::endl;
                //std::cout << "size_pre : " << size_pre << std::endl;
                bc_[size_pre].base = -1 * f_index; // 中間をつなげる
                bc_[f_index].check = size_pre;

                for(int i = size_pre; i < size; i++) {
                    if(i > size_pre) {
                        bc_[i].base = -1 * (i-1);
                    }
                    bc_[i].check = i + 1;
                    bc_[i].is_used = true;
                }
                bc_[size-1].check = e_index;
                bc_[e_index].base = -1 * (size-1);
            }
        }
    }

    // 残りの文字列を新たに格納
    void InsertSuffix(int r, std::string_view ax) {
        //std::cout << "-------------InsertSuffix-----------------" << std::endl;
        std::unordered_map<uint8_t, int> row;
        int node = r;
        for (int i =0; i < ax.size(); i++) {
            uint8_t c = ax[i];
            row[c] = 0;
            int base = find_base(row);
            bc_[node].base = base;
            int next_index = base + c;
            expand(next_index);
            AddCheck(next_index, node);
            //bc_[next_index].check = node;
            node = next_index;
            row.clear();
        }
        //std::cout << "way to InsertSuffix" << std::endl;
        // kLeafChar
        row[kLeafChar] = 0;
        int base = find_base(row);
        bc_[node].base = base;
        int next_index = base + kLeafChar;
        expand(next_index);
        AddCheck(next_index, node);
        //bc_[next_index].check = node;
        row.clear();
        //std::cout << "---------------end of InsertSuffix------------------" << std::endl;
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
            AddCheck(next_index, r);
            //bc_[next_index].check = r;
            assert(tmp_base.count(a) == 1);
            bc_[next_index].base = tmp_base.find(a)->second;
            //子の子のcheck値を付け替える
            assert(tmp_index.count(a) == 1);
            int index = tmp_index.find(a)->second;
            if(a != kLeafChar) {
                for(int j=0; j < tmp_child.size(); j++) {
                    if(tmp_child[j][index]) {
                        int next_next_index = bc_[next_index].base + tmp_child[j][index];
                        AddCheck(next_next_index, next_index);
                        //bc_[next_next_index].check = next_index;
                    }
                }
                /*
                for(int i = 1; i < MaxUint8_t; i++) {
                    uint8_t b = i;
                    int next_next_index = bc_[next_index].base + b;
                    if (bc_[next_next_index].check == index) {
                        AddCheck(next_next_index, next_index);
                        //bc_[next_next_index].check = next_index;
                    }
                }
                */
            }
            if (a != kLeafChar) {
                int next_next_index = bc_[next_index].base + kLeafChar;
                if (bc_[next_next_index].check == index) {
                    AddCheck(next_next_index, next_index);
                    //bc_[next_next_index].check = next_index;
                }
            }
        }
        // uint8_t c の追加
        if (c != kLeafChar) {
            int next_index = base + c;
            expand(next_index);
            AddCheck(next_index, node);
            //bc_[next_index].check = node;
            node = next_index;
            InsertSuffix(node, str);
        }
    }

    // 親番号に対する子の集合を返すための関数
    std::vector<uint8_t> GetChildren(int r) {
        std::vector<uint8_t> row;
        int count = 0;
        if(bc_[r].base == kEmptyBase) {
            return row;
        }
        //std::cout << "a" <<std::endl;
        for(int i = 1; i < MaxUint8_t; i++) {
            uint8_t c = i;
            int idx = bc_[r].base + c;
            //if(r == bc_[idx].check) {
            if (bc_.size() > idx) {
                if(r == bc_[idx].check) {
                //if (bc_.size() > idx) {
                    //row[c] = 0;
                    row.emplace_back();
                    row[count] = c;
                    count++;
                }
            }
        }
        //std::cout << "b" <<std::endl;

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

    // 使っている要素を削除して，未使用要素に連結する
    void Delete(int index) {
        //std::cout << "------- Delete ------" << std::endl;
        //std::cout << "index : " << index << std::endl;
        //std::cout << "is_used : " << bc_[index].is_used << std::endl;
        if(E_HEAD == -1) {
            E_HEAD = index;
            bc_[index].is_used = true;
            bc_[index].check = index;
            bc_[index].base = -1 * index;
        }
        else {
            int e_index = E_HEAD;
            bc_[index].is_used = true;
            bool flag = true;
            
            int prev_index = -1 * bc_[e_index].base;
            int next_index = bc_[e_index].check;
            //std::cout << "E_HEAD : " << E_HEAD << std::endl;
            //std::cout << "prev_index : " << prev_index << std::endl;
            //std::cout << "next_index : " << next_index << std::endl;
            bc_[e_index].check = index;
            bc_[next_index].base = -1 * index;
            bc_[index].check = next_index;
            bc_[index].base = -1 * e_index;
            E_HEAD = index;
            //std::cout << "E_HEAD : " << E_HEAD << std::endl;
        }
    }

    // bc_[index].checkにvalを格納する
    // 未使用要素を再構築する
    void AddCheck(int index, int val) {
        //std::cout << "------- AddCheck ------" << std::endl;
        //std::cout << "index : " << index << std::endl;
        //std::cout << "bc_.size : " << bc_.size() << std::endl;
        int e_index = E_HEAD;
        bool flag = true;
        
        if(bc_[index].is_used == false) {
            //flag = false;
            E_HEAD = -1;
            bc_[index].check = val;
            return;
        }

        bc_[index].is_used = false; // 使っている
        
        if(E_HEAD == index) {// 未使用要素の先頭だった場合
            E_HEAD = bc_[index].check;
            int prev_index = -1 * bc_[index].base;
            int next_index = bc_[index].check;
            //std::cout << "prev_index : " << prev_index << std::endl;
            //std::cout << "next_index : " << next_index << std::endl;
            bc_[prev_index].check = next_index;
            bc_[next_index].base = -1 * prev_index;
            flag = false;
        }
        if(flag) {
            int prev_index = -1 * bc_[index].base;
            int next_index = bc_[index].check;
            bc_[prev_index].check = next_index;
            bc_[next_index].base = -1 * prev_index;
            //std::cout << "prev_index : " << prev_index << std::endl;
            //std::cout << "next_index : " << next_index << std::endl;
            //std::cout << "next_index_flag : " << bc_[next_index].is_used << std::endl;
        }
        bc_[index].check = val;

        // 要素をすべて使い切ってしまった時の処理
        if(bc_[E_HEAD].is_used == false) {
            //std::cout << "-------- run out of -------" << std::endl;
            //std::cout << "here" << std::endl;
            //E_HEAD = bc_.size();
            //std::cout << "here2" << std::endl;
            //std::cout << "E_HEAD : " << E_HEAD << std::endl;
            //std::cout << "e_index : " << e_index << std::endl;
            //std::cout << "flag1 : " << bc_[E_HEAD].is_used << std::endl;
            //std::cout << "flag2 : " << bc_[e_index].is_used << std::endl;
            //expand(bc_.size());
            E_HEAD = -1;
            //expand(bc_.size()+1);
            //std::cout << "here3" << std::endl;
        }
    }

    // 遷移が成功するかどうかの判定(成功する場合は次の親番号を返値, 失敗した場合は0を返値)
    int Transition(int r, uint8_t c) const {
        auto base = bc_[r].base;
        if (base == kEmptyBase)
            return kFailedIndex;
        int t = base + c;
        return t < bc_.size() && bc_[t].check == r ? t : kFailedIndex;
    }
};

}