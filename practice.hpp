#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <string_view>
#include <cassert>
#include <thread>

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
        bool not_used;
        uint8_t child, sibling;
        Unit() : base(kEmptyBase), check(kEmptyCheck), not_used(true), child(MaxUint8_t), sibling(MaxUint8_t) {}
        Unit(int b, int c, bool f, uint8_t ch, uint8_t s) : base(b),check(c),not_used(f),child(ch),sibling(s) {}
    };
    struct Tmp {
        int index;
        uint8_t child, sibling;
    };
    // 子の子を保存するための構造体
    struct ChildUint {
        int index;
        uint8_t child;
    };
    std::vector<Unit> bc_;
    int E_HEAD = -1;
    std::vector<ChildUint> tmp_arr; // 子の子を保存する
    std::vector<uint8_t> TAIL;
    

public:
    //StringSet() = default;
    StringSet() {
        bc_ = {{kEmptyBase, 0, false, MaxUint8_t, MaxUint8_t}}; // set root element
        TAIL.resize(1, MaxUint8_t-1);
    }

    // 文字列を追加するための関数
    void insert(const std::string& str) {
        int node = 0;
        int n = 0;
        for(uint8_t c : str) {
            // TAILに対する遷移を検索するための処理
            if(bc_[node].base < 0) {
                int base = -1 * bc_[node].base;
                int loop_count = 0;
                
                for(int i=n; i < str.size(); i++, loop_count++) {
                    uint8_t cc = str[i];
                    //if(TAIL[base+loop_count] != str[i]) {
                    if(TAIL[base+loop_count] != cc) {
                        //std::cout << "Failed c : " << str[i] << std::endl;
                        //std::cout << "here 1" << std::endl;
                        InsertSuffixAndTAIL(node, std::string_view(str).substr(n), loop_count);
                        break;
                    }
                }
                break;
            }

            int next_node = Transition(node, c);
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
                    //expand(next_index);
                    if(bc_[next_index].not_used == true) {
                        expand(next_index);
                        AddCheck(next_index, node);
                        //bc_[next_index].check = node;
                        //bc_[next_index].not_used = false;
                        //std::cout << "children exists in space" << std::endl;
                        int next_node = bc_[node].base + bc_[node].child;
                        //std::cout << "child : " << bc_[node].child << std::endl;
                        // siblingに対して，値の追加
                        while(true) {
                            //std::cout << "----------------" << std::endl;
                            //std::cout << "node    : " << next_node << std::endl;
                            //std::cout << "not_used: " << bc_[next_node].not_used << std::endl;
                            //std::cout << "child   : " << bc_[next_node].child << std::endl;
                            //std::cout << "child   : " << int(bc_[next_node].child) << std::endl;
                            //std::cout << "sibling : " << bc_[next_node].sibling << std::endl;
                            //std::cout << "sibling : " << int(bc_[next_node].sibling) << std::endl;
                            //if(bc_[next_node].sibling == MaxUint8_t and bc_[next_node].child == MaxUint8_t) {
                            if(bc_[next_node].sibling == MaxUint8_t) {
                                bc_[next_node].sibling = c;
                                break;
                            }
                            next_node = bc_[node].base + bc_[next_node].sibling;
                        }
                        //InsertSuffix(next_index, std::string_view(str).substr(n+1));
                        InsertTAIL(next_index, std::string_view(str).substr(n+1));
                        //InsertSuffix(next_index, std::string_view(str).substr(n+1));
                        break;
                    }
                    else {
                        // いろいろな値の保存
                        for(int i=0; i < row.size(); i++) {
                            uint8_t a = row[i];
                            int index = bc_[node].base + a;

                            // 子の子の値の保存
                            bool is_child = false;
                            for(int i=0; i < tmp_arr.size(); i++) {
                                if(tmp_arr[i].index == index) {
                                    is_child = true;
                                }
                            }
                            if(!is_child) {
                                uint8_t c1 = bc_[index].child;
                                if(c1 == MaxUint8_t) {
                                    continue;
                                }

                                int arr_size = tmp_arr.size();
                                tmp_arr.emplace_back();
                                tmp_arr[arr_size].index = index;
                                tmp_arr[arr_size].child = c1;

                                int next = bc_[index].base + c1;
                                while(bc_[next].sibling != MaxUint8_t) {
                                    c1 = bc_[next].sibling;
                                    next = bc_[index].base + c1;
                                    int arr_size = tmp_arr.size();
                                    tmp_arr.emplace_back();
                                    tmp_arr[arr_size].index = index;
                                    tmp_arr[arr_size].child = c1;
                                }
                            }

                            // 使わない部部の消去
                            //Delete(index);
                        }
                        //std::cout << "children exists not space" << std::endl;
                        ModifyAndInsertSuffix(node, row, c, std::string_view(str).substr(n+1));
                        tmp_arr.clear();
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
        int n = 0;
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
                    std::cout << "flag : " << bc_[tmp_base].not_used << std::endl;
                    int tmp_next = bc_[tmp_base].base + a;
                    std::cout << "check : " << bc_[tmp_next].check << std::endl;
                    if(tmp_base != bc_[tmp_next].check) {
                        break;
                    }
                    tmp_base = tmp_next;
                }
                return false;
            }
            if(bc_[next_node].base < 0) {
                //std::cout << "c : " << c << std::endl;
                //std::cout << "n : " << n << std::endl;
                //std::cout << "key.size : " << key.size() << std::endl;
                int next = -1 * bc_[next_node].base;
                //std::cout << "TAIL : " << TAIL[next] << std::endl;
                
                // 検索はn+1から行う
                int loop = 0;
                //std::cout << "next : " << next << std::endl;
                //std::cout << "key : " << key[n+1] << std::endl;
                for(int i=n+1; i < key.size(); i++, loop++) {
                    uint8_t a = key[i];
                    //std::cout << "---------" << std::endl;
                    //std::cout << "a : " << int(a) << std::endl;
                    //std::cout << "TAIL : " << int(TAIL[next+loop]) << std::endl;
                    if(a != TAIL[next+loop]) {
                        //std::cout << "i : " << i << std::endl;
                        std::cout << "key : " << key << std::endl;
                        std::cout << "key[i] : " << key[i] << std::endl;
                        std::cout << "TAIL[] : " << TAIL[next+loop] << std::endl;
                        int loop2 = 0;
                        while(true) {
                            //std::cout << TAIL[next+loop2] << std::endl;
                            if(TAIL[next+loop2] == kLeafChar) {
                                break;
                            }
                            loop2++;
                        }
                        return false;
                    }
                }
                //std::cout << "value : " << TAIL[next + loop] << std::endl;
                //std::cout << next + loop << std::endl;
                if(kLeafChar == TAIL[next + loop]) {
                    //std::cout << "success" << std::endl;
                    return true;
                }
                else {
                    return false;
                }
            }
            n++;
            node = next_node;
        }
        // '\0'
        int next_node = Transition(node, kLeafChar);
        //std::cout << "--------kLeafer check---------------------" << std::endl;
        //std::cout << "node  : " << node << std::endl;
        //std::cout << "check : " << bc_[next_node].check << std::endl;
        return next_node != kFailedIndex;
        /*
        int next = -1 * bc_[next_node].base;
        if(MaxUint8_t == TAIL[next]) {
            return true;
        }
        else {
            return false;
        }
        */
    }

    void SizeCheck() {
        std::cout << "bc_size : " << bc_.size() << std::endl;
    }

    void CheckContent() {
        std::cout << "----- CheckContent -----" << std::endl;
        for(int i=0; i < bc_.size(); i++) {
            if(bc_[i].not_used == true) {
                std::cout << "check : " << bc_[i].check << ", i : " << i << std::endl;
            }
        }
        std::cout << "-------------base--------------" << std::endl;
        for(int i=0; i < bc_.size(); i++) {
            if(bc_[i].not_used == true) {
                std::cout << "base : " << bc_[i].base << ", i : " << i << std::endl;
            }
        }
    }

    // TAILの中身を確認する
    void CheckTAIL() {
        /*
        std::cout << "TAIL : ";
        for(int i=0; i < TAIL.size(); i++) {
            std::cout << TAIL[i];
        }
        std::cout << std::endl;
        */
        /*
        std::cout << "---- CheckTAIL ----" << std::endl;
        for(int i = 0; i < TAIL.size(); i++) {
            //std::cout << TAIL[i] << ", " << int(TAIL[i]) << ", " << i << std::endl;
            //std::cout << "TAIL[i] : " << int(TAIL[i]) << std::endl;
            std::cout << "TAIL[i] : " << TAIL[i] << std::endl;
        }
        */
        std::cout << "TAIL_size : " << TAIL.size() << std::endl;
        int count = 0;
        for(int i=0; i < TAIL.size(); i++) {
            if(TAIL[i] == MaxUint8_t-1) {
                count++;
            }
        }
        std::cout << "not use rate : " << count << " / " << TAIL.size() << std::endl;
    }

    // 空要素数を求める関数
    void EmptyNum() {
        int size = bc_.size();
        int empty_num = 0;
        for(int i=0; i < size; i++) {
            if(bc_[i].not_used == true) {
                empty_num++;
            }
        }
        std::cout << "not use element num : " << empty_num << " / " << size << std::endl;
    }

private:
    int find_base(const std::vector<uint8_t>& row) const {
        //std::cout << "--------------find_base----------------" << std::endl;
        int e_index = E_HEAD;
        int base;
        int roop = 0;
        
        if(E_HEAD == -1) {
            return bc_.size();
        }

        // row の中で，一番小さい数字を選択
        uint8_t c = MaxUint8_t;
        if(row.size() == 0) {
            c = kLeafChar;
        }
        
        for(uint8_t a : row) {
            if(c > a) {
                c = a;
            }
        }
        if(bc_[e_index].not_used == false) {
            return bc_.size();
        }
        //std::cout << "process find base" << std::endl;
        while(true) {
            //std::cout << "--------while------" << std::endl;
            //std::cout << "bc_size : " << bc_.size() << std::endl;
            //std::cout << "e_index : " << e_index << std::endl;
            //std::cout << "e_index-not_used : " << bc_[e_index].not_used << std::endl;
            //std::cout << "e_index-base : " << bc_[e_index].check << std::endl;

            roop += 1;
            bool found = true;
            base = e_index - c;
            
            if(base > 0) {
                for(uint8_t b : row) {
                    if((base + b) >= bc_.size()) {
                        continue;
                    }
                    if(bc_[base + b].not_used == false) { // falseは使われているという意味
                        e_index = bc_[e_index].check;
                        if(e_index == E_HEAD) {
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
        return bc_.size() - c;
    }

    void expand(int index) {
        //std::cout << "----------------expand---------------" << std::endl;
        //std::cout << "pre_size : " << bc_.size() << std::endl;
        if (index < bc_.size())
            return;
        
        int size_pre = bc_.size();
        //std::cout << "flag : " << bc_[E_HEAD].not_used << std::endl;
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
                bc_[i].not_used = true;
            }
            bc_[size-1].check = size_pre;
            bc_[size_pre].base = -1 * (size-1);
        }
        else {
            int e_index = E_HEAD;

            if(bc_[e_index].not_used == false) {
                //std::cout << "------------------------- flag false -----------------------" << std::endl;
                //std::cout << "e_index : " << e_index << std::endl;
                //std::cout << "flag : " << bc_[e_index].not_used << std::endl;
                //std::cout << "size_pre : " << size_pre << std::endl;
                //std::cout << "size : " << size << std::endl;
                for(int i=size_pre; i < size; i++) {
                    if(i > size_pre) {
                        bc_[i].base = -1 * (i-1);
                    }
                    bc_[i].check = i+1;
                    bc_[i].not_used = true;
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
                    bc_[i].not_used = true;
                }
                bc_[size-1].check = e_index;
                bc_[e_index].base = -1 * (size-1);
            }
        }
    }

    // 残りの文字列を新たに格納
    void InsertSuffix(int r, std::string_view ax) {
        //std::cout << "-------------InsertSuffix-----------------" << std::endl;
        std::vector<uint8_t> row;
        int node = r;
        /*
        for (int i =0; i < ax.size(); i++) {
            uint8_t c = ax[i];
            row.push_back(c);
            int base = find_base(row);
            bc_[node].base = base;
            if(bc_[node].child == MaxUint8_t) {
                bc_[node].child = c;
            }
            int next_index = base + c;
            expand(next_index);
            AddCheck(next_index, node);
            //bc_[next_index].check = node;
            bc_[next_index].sibling = MaxUint8_t;
            node = next_index;
            row.clear();
        }
        //std::cout << "way to InsertSuffix" << std::endl;
        // kLeafChar
        row.push_back(kLeafChar);
        int base = find_base(row);
        bc_[node].base = base;
        bc_[node].child = kLeafChar;
        int next_index = base + kLeafChar;
        expand(next_index);
        AddCheck(next_index, node);
        bc_[next_index].sibling = MaxUint8_t;
        //bc_[next_index].check = node;
        row.clear();
        //std::cout << "---------------end of InsertSuffix------------------" << std::endl;
        */
        uint8_t c = ax[0];
        row.push_back(c);
        int base = find_base(row);
        bc_[node].base = base;
        bc_[node].child = c;
        //auto row2 = GetChildren(node);
        //std::cout << "size : " << row2.size() << std::endl;
        //std::cout << "row2 : " << row2[0] << std::endl;
        int next_index = base + c;
        expand(next_index);
        AddCheck(next_index, node);
        node = next_index;
        //row.clear();

        InsertTAIL(node, std::string_view(ax).substr(1));
    }

    // TAILに格納するための関数(削除は考えずにすべて後ろに追加していく)
    void InsertTAIL(int r, std::string_view ax) {
        //std::cout << "ax : " << ax << ", " << ax.size() << std::endl;
        int pre_size = TAIL.size();
        //std::cout << "pre_size : " << pre_size << std::endl;
        
        bc_[r].base = -1 * pre_size;
        
        TAIL.resize(pre_size+ax.size()+1, MaxUint8_t-1);
        //std::cout << "TAIL.size : " << TAIL.size() << std::endl;
        //std::cout << "ax.size : " << ax.size() << std::endl;
        if(ax.size() > 0) {
            for(int i=0; i < ax.size(); i++) {
                TAIL[pre_size+i] = ax[i];
                //std::cout << "pre_size + i : " << pre_size+i << std::endl;
                //std::cout << "ax : " << int(ax[i]) << std::endl;
                //std::cout << "TAIL[pre_size+i] : " << TAIL[pre_size+i] << std::endl;
            }
            TAIL[TAIL.size()-1] = kLeafChar;
        }
        else {
            TAIL.resize(pre_size + 1);
            TAIL[TAIL.size()-1] = MaxUint8_t;
        }
        //std::cout << "TAIL.size -1 : " << TAIL.size()-1 << std::endl;
    }

    void InsertSuffixAndTAIL(int r, std::string_view ax, int num) {
        //std::cout << "---------- InsertSuffixAndTAIL ----------" << std::endl;
        int TAIL_node = -1 * bc_[r].base; // TAILの先頭を示す
        int node = r;
        //std::cout << "r   : " << r << std::endl;
        //std::cout << "ax  : " << ax << std::endl;
        //std::cout << "num : " << num << std::endl;
        //std::cout << "TAIL : " << TAIL[node+num] << std::endl;
        //std::cout << "str  : " << ax[num] << std::endl;
        std::vector<uint8_t> row;
        std::string tmp_TAIL = "";
        int loop = 0;
        while(true) {
            tmp_TAIL += TAIL[TAIL_node + loop];
            //std::cout << TAIL[TAIL_node + loop] << ", " << int(TAIL[TAIL_node + loop]) << std::endl;;
            if(TAIL[TAIL_node + loop] == kLeafChar) {
                break;
            }
            // 使わない部分の削除
            //TAIL[TAIL_node + loop] = MaxUint8_t-1;
            loop++;
        }
        for(int i=0; i < num; i++) {
            uint8_t c = ax[i];
            row.push_back(c);
            int base = find_base(row);
            bc_[node].base = base;
            bc_[node].child = c;
            //bc_[node].sibling = MaxUint8_t;
            int next_index = base + c;
            expand(next_index);
            AddCheck(next_index, node);
            node = next_index;
            row.clear();
        }
        // 分岐している部分の処理
        //uint8_t c1 = TAIL[TAIL_node + num];
        uint8_t c1 = tmp_TAIL[num];
        uint8_t c2;
        if(ax.size() == 0) {
            c2 = kLeafChar;
        }
        else {
            c2 = ax[num];
        }
        row.push_back(c1);
        row.push_back(c2);
        //std::cout << "c1, c2 : " << c1 << ", " << c2 << std::endl;
        //std::cout << int(c1) << ", " << int(c2) << std::endl;
        int base = find_base(row);
        bc_[node].base = base;
        bc_[node].child = c1;
        int next_index = base + c1;
        expand(next_index);
        AddCheck(next_index, node);
        bc_[next_index].sibling = c2;
        next_index = base + c2;
        expand(next_index);
        AddCheck(next_index, node);
        node = base + c1;
        bc_[node].base = -1 * (TAIL_node + num + 1);
        //InsertTAIL(node, std::string_view(tmp_TAIL).substr(num+1));
        node = base + c2;
        InsertTAIL(node, std::string_view(ax).substr(num+1));

    }

    // 更新
    void ModifyAndInsertSuffix(int r, 
        const std::vector<uint8_t>& row, 
        uint8_t c, 
        std::string_view str) 
        {
        int node = r;
        std::vector<uint8_t> tmp_row = row;
        uint8_t prev_a;
        std::vector<int> next;
        std::vector<Tmp> tmp_;

        int pre_base = bc_[r].base; // 基準とするためのbase値の保存
        tmp_row.push_back(c);
        int base = find_base(tmp_row);
        bc_[r].base = base;
        //std::cout << "size  : " << row.size() << std::endl;
        //std::cout << "child : " << int(bc_[r].child) << std::endl;
        for (int i = 0; i < row.size(); i++) {
            uint8_t a = row[i];
            int next_index = base + a;
            //std::cout << "------------------" << std::endl;
            //std::cout << "a : " << a << std::endl;
            //std::cout << "index : " << pre_base + a << std::endl;
            //std::cout << "not_used: " << bc_[pre_base + a].not_used << std::endl;
            //std::cout << "child   : " << int(bc_[pre_base + a].child) << std::endl;
            //std::cout << "sibling : " << bc_[pre_base + a].sibling << std::endl;
            expand(next_index);
            AddCheck(next_index, r);
            /*
            assert(tmp_base.count(a) == 1);
            //bc_[next_index].base = tmp_base.find(a)->second;
            int base2 = tmp_base.find(a)->second;
            bc_[next_index].base = base2;
            assert(tmp_index.count(a) == 1);
            int index = tmp_index.find(a)->second;
            */
            int base2, index;

            index = pre_base + a;
            base2 = bc_[index].base;
            bc_[next_index].base = base2;

            // 子リンクと兄弟リンクの遷移を行う
            //uint8_t child2 = bc_[base2 + a].child;
            //uint8_t sibling2 = bc_[base2 + a].sibling;
            //if(bc_[index].sibling == MaxUint8_t and bc_[index].child != MaxUint8_t and a != bc_[r].child) {
            if(bc_[index].sibling == MaxUint8_t and a != bc_[r].child) {
                prev_a = a;
            }

            //子の子のcheck値を付け替える
            int size = next.size();
            next.resize(size+1);
            tmp_.resize(size+1);
            next[size] = next_index;
            tmp_[size].index = index;
            tmp_[size].child = bc_[index].child;
            tmp_[size].sibling = bc_[index].sibling;

            if(a != kLeafChar) {
                for(int j=0; j < tmp_arr.size(); j++) {
                    if(tmp_arr[j].index == index) {
                        int next_next_index = bc_[next_index].base + tmp_arr[j].child;
                        AddCheck(next_next_index, next_index);
                        tmp_arr.erase(tmp_arr.begin() + j);
                        j--;
                    }
                }
            }
            /*
            if (a != kLeafChar) {
                int next_next_index = bc_[next_index].base + kLeafChar;
                if (bc_[next_next_index].check == index) {
                    AddCheck(next_next_index, next_index);
                }
            }
            */
            // 使わない部部の消去
            Delete(index);
        }

        // 子供が一つしか存在しない場合
        if(row.size() == 1) {
            prev_a = bc_[r].child;
        }
        // 削除から
        for(int i=0; i < next.size(); i++) {
            int index = tmp_[i].index;
            bc_[index].child = MaxUint8_t;
            bc_[index].sibling = MaxUint8_t;
        }
        // 付け替える
        for(int i=0; i < next.size(); i++) {
            int index = tmp_[i].index;
            int next_index = next[i];
            bc_[next_index].child = tmp_[i].child;
            bc_[next_index].sibling = tmp_[i].sibling;
        }
        // uint8_t c の追加
        if (c != kLeafChar) {
            int next_index = base + c;
            expand(next_index);
            AddCheck(next_index, node);
            //bc_[next_index].check = node;
            bc_[base + prev_a].sibling = c;
            bc_[base + c].sibling = MaxUint8_t;
            node = next_index;
            //InsertSuffix(node, str);
            InsertTAIL(node, std::string_view(str).substr(0));
        }
        next.clear();
        tmp_.clear();
    }

    // 親番号に対する子の集合を返すための関数
    std::vector<uint8_t> GetChildren(int r) {
        std::vector<uint8_t> row;
        int count = 0;
        if(bc_[r].base == kEmptyBase) {
            return row;
        }

        int next_node = bc_[r].base + bc_[r].child;
        row.emplace_back();
        row[count] = bc_[r].child;
        count++;

        while(true) {
            if(bc_[next_node].sibling == MaxUint8_t) {
                return  row;
            }
            row.emplace_back();
            row[count] = bc_[next_node].sibling;
            next_node = bc_[r].base + bc_[next_node].sibling;
            count++;
        }
    }

    // 使っている要素を削除して，未使用要素に連結する
    void Delete(int index) {
        //std::cout << "------- Delete ------" << std::endl;
        //std::cout << "index : " << index << std::endl;
        //std::cout << "not_used : " << bc_[index].not_used << std::endl;
        if(E_HEAD == -1) {
            E_HEAD = index;
            bc_[index].not_used = true;
            bc_[index].check = index;
            bc_[index].base = -1 * index;
        }
        else {
            int e_index = E_HEAD;
            bc_[index].not_used = true;
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
        
        if(bc_[index].not_used == false) {
            //flag = false;
            E_HEAD = -1;
            bc_[index].check = val;
            return;
        }

        bc_[index].not_used = false; // 使っている
        
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
            //std::cout << "next_index_flag : " << bc_[next_index].not_used << std::endl;
        }
        bc_[index].check = val;

        // 要素をすべて使い切ってしまった時の処理
        if(bc_[E_HEAD].not_used == false) {
            E_HEAD = -1;
        }
    }

    // 遷移が成功するかどうかの判定(成功する場合は次の親番号を返値, 失敗した場合は0を返値)
    int Transition(int r, uint8_t c) const {
        auto base = bc_[r].base;
        if(bc_[r].base < 0) {
            int base = -1 * bc_[r].base;
            if(base > TAIL.size()) {
                return kFailedIndex;
            }
            else {
                return kEmptyBase;
            }
        }
        if (base == kEmptyBase)
            return kFailedIndex;
        int t = base + c;
        return t < bc_.size() && bc_[t].check == r ? t : kFailedIndex;
    }
};

}