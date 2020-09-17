#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <string_view>
#include <cassert>
#include <thread>
#include <algorithm>

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
    // rebuildする際に用いる
    struct IndexUint {
        int pre_index; // 再構築前の値の保持
        int now_index; // 再構築後の値の保持
    };
    std::vector<Unit> bc_;
    int E_HEAD = -1;
    std::vector<uint8_t> TAIL;
    std::vector<Unit> tmp_bc_;
    

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
                    if(TAIL[base+loop_count] != cc) {
                        InsertSuffixAndTAIL(node, std::string_view(str).substr(n), loop_count);
                        break;
                    }
                }
                break;
            }

            int next_node = Transition(node, c);
            if (next_node == kFailedIndex) {

                auto row = GetChildren(node); // 子を取得
                uint8_t v = str[n];
                
                // 子が存在するとき
                int next_index = bc_[node].base + c;
                    
                if(bc_[next_index].not_used) { // 追加する場所が未使用要素の時
                    expand(next_index);
                    AddCheck(next_index, node);
                    // siblingに対して，値の追加
                    bc_[next_index].sibling = bc_[node].child;
                    bc_[node].child = c;

                    InsertTAIL(next_index, std::string_view(str).substr(n+1));
                    break;
                }
                else {
                    std::vector<ChildUint> tmp_arr; // 子の子を保存する
                    // いろいろな値の保存
                    for(uint8_t a : row) {
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
                    }
                    //std::cout << "children exists not space" << std::endl;
                    ModifyAndInsertSuffix(node, row, c, std::string_view(str).substr(n+1), tmp_arr);
                    break;
                }
            }
            node = next_node;
            n++;
        }
    }

    bool contains(const std::string& key) const {
        //std::cout << "--------------contains---------------" << std::endl;
        int node = 0; // root
        int pos = 0;
        int next_node;
        
        // ダブル配列内のチェック
        for(uint8_t c : key) {
            next_node = Transition(node, c);
            if (next_node == kFailedIndex)
                return false;
            if(bc_[next_node].base < 0)
                break;
            pos++;
            node = next_node;
        }
        // TAIL内のチェック
        if(pos < key.size()) {
            int next = -1 * bc_[next_node].base;
            int loop = 0;
            for(int i=pos+1; i < key.size(); i++, loop++) {
                uint8_t a = key[i];
                if(a != TAIL[next+loop]) {
                    return false;
                }
            }
            if(kLeafChar == TAIL[next+loop])
                return true;
       }
       // 終端文字の検索
       else {
           next_node = Transition(node, kLeafChar);
           return next_node != kFailedIndex;
       }
       return false;
    }

    void SizeCheck() {
        std::cout << "bc_size : " << bc_.size() << std::endl;
    }

    void CheckContent() {
        std::cout << "----- CheckContent -----" << std::endl;
        for(int i=0; i < bc_.size(); i++) {
            if(bc_[i].not_used) {
                std::cout << "check : " << bc_[i].check << ", i : " << i << std::endl;
            }
        }
        std::cout << "-------------base--------------" << std::endl;
        for(int i=0; i < bc_.size(); i++) {
            if(bc_[i].not_used) {
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
        std::cout << "not use TAIL rate : " << count << " / " << TAIL.size() << std::endl;
    }

    // 空要素数を求める関数
    void EmptyNum() {
        int size = bc_.size();
        int empty_num = 0;
        for(int i=0; i < size; i++) {
            if(bc_[i].not_used) {
                empty_num++;
            }
        }
        std::cout << "not use element num : " << empty_num << " / " << size << std::endl;
    }

    // REBUILD関数(静的にbc_を作り直す)
    void REBUILD() {
        std::cout << "REBUILD" << std::endl;
        tmp_bc_ = bc_; // 元のデータをtmp_bcにコピーしておく
        std::vector<IndexUint> tmp_index; // 遷移ができるindexを格納しておくための配列(もともとのダブル配列のindex)
        bc_.clear();
        E_HEAD = -1; // 初期化したため
        bc_ = {{kEmptyBase, 0, false, MaxUint8_t, MaxUint8_t}}; // set root element
        expand(tmp_bc_.size()-1); // expand関数を用いることによって，領域を確保し，未使用要素を連結している
        
        // 先頭ノードのみ行う(tmp_indexのサイズが0だから)
        auto row = GetChildrenRebild(0); // 専用の関数(REBUILDの時のみ用いる)
        int base = find_base(row);
        bc_[0].base = base;
        bc_[0].child = tmp_bc_[0].child;
        bc_[0].sibling = tmp_bc_[0].sibling;
        for(auto c : row) {
            int next_node = tmp_bc_[0].base + c;
            int next_node_now = base + c;
            AddCheck(next_node_now, 0);
            bc_[next_node_now].child = tmp_bc_[next_node].child;
            bc_[next_node_now].sibling = tmp_bc_[next_node].sibling;
            if(tmp_bc_[next_node].base < 0) {
                bc_[base + c].base = tmp_bc_[next_node].base;
            }
            else {
                int size = tmp_index.size();
                tmp_index.resize(size+1);
                tmp_index[size].pre_index = next_node;
                tmp_index[size].now_index = next_node_now;
            }
        }
        // tmp_indexは，使ったら消す処理をしているので，要素がなくなるまで
        while(tmp_index.size() != 0) {
            int pre_node = tmp_index[0].pre_index;
            int now_node = tmp_index[0].now_index;
            auto row = GetChildrenRebild(pre_node);
            int base = find_base(row);
            bc_[now_node].base = base;
            for(auto c : row) {
                int next_node = tmp_bc_[pre_node].base + c;
                int next_node_now = base + c;
                AddCheck(base+c, now_node);
                bc_[next_node_now].child = tmp_bc_[next_node].child;
                bc_[next_node_now].sibling = tmp_bc_[next_node].sibling;
                if(tmp_bc_[next_node].base < 0) {
                    bc_[base + c].base = tmp_bc_[next_node].base;
                }
                else {
                    int size = tmp_index.size();
                    tmp_index.resize(size+1);
                    tmp_index[size].pre_index = next_node;
                    tmp_index[size].now_index = base + c;
                }
            }
            tmp_index.erase(tmp_index.begin()); // 先頭の要素を消している
        }
        tmp_bc_.clear();
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
        uint8_t c = *std::min_element(row.begin(), row.end()); // 値ではなく，反復子を返す(*がいる)
        
        while(true) {

            roop += 1;
            bool found = true;
            base = e_index - c;
            
            if(base > 0) {
                for(uint8_t b : row) {
                    if((base + b) >= bc_.size()) {
                        continue;
                    }
                    if(!bc_[base + b].not_used) { // falseは使われているという意味
                        found = false;
                        break;
                    }
                }
            }
            else {
                found = false;
            }
            if(found) { // 場所が見つかった時
                return base;
            }
            // 次の要素を検索するための処理
            e_index = bc_[e_index].check;
            if(e_index == E_HEAD) {
                break;
            }
        }
        base = bc_.size() - c;
        if(base < 0) {
            base = 0;
        }
        return base;
    }

    void expand(int index) {
        //std::cout << "----------------expand---------------" << std::endl;
        
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

            if(!bc_[e_index].not_used) {
                //std::cout << "------------------------- flag false -----------------------" << std::endl;
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
    void InsertSuffix(int r, std::string_view left_str) {
        //std::cout << "-------------InsertSuffix-----------------" << std::endl;
        std::vector<uint8_t> row;
        int node = r;
        
        uint8_t c = left_str[0];
        row.push_back(c);
        int base = find_base(row);
        bc_[node].base = base;
        bc_[node].child = c;
        
        int next_index = base + c;
        expand(next_index);
        AddCheck(next_index, node);
        node = next_index;

        InsertTAIL(node, std::string_view(left_str).substr(1));
    }

    // TAILに格納するための関数
    void InsertTAIL(int r, std::string_view ax) {
        
        int pre_size = TAIL.size();
        bc_[r].base = -1 * pre_size;
        TAIL.resize(pre_size+ax.size()+1, MaxUint8_t-1);
        
        for(int i=0; i < ax.size(); i++) {
            TAIL[pre_size+i] = ax[i];
        }
        TAIL[TAIL.size()-1] = kLeafChar;
    }

    void InsertSuffixAndTAIL(int r, std::string_view ax, int num) {
        //std::cout << "---------- InsertSuffixAndTAIL ----------" << std::endl;
        int TAIL_node = -1 * bc_[r].base; // TAILの先頭を示す
        int node = r;
        std::vector<uint8_t> row;
        int loop = 0;
        
        for(int i=0; i < num; i++) {
            uint8_t c = ax[i];
            row.push_back(c);
            int base = find_base(row);
            bc_[node].base = base;
            bc_[node].child = c;
            int next_index = base + c;
            expand(next_index);
            AddCheck(next_index, node);
            node = next_index;
            row.clear();
        }
        // 分岐している部分の処理
        uint8_t c1 = TAIL[TAIL_node + num];
        uint8_t c2;
        if(ax.size() == 0) {
            c2 = kLeafChar;
        }
        else {
            c2 = ax[num];
        }
        row.push_back(c1);
        row.push_back(c2);
        
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
        std::string_view str,
        std::vector<ChildUint> tmp_arr) 
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
        
        for (int i = 0; i < row.size(); i++) {
            uint8_t a = row[i];
            int next_index = base + a;
            
            expand(next_index);
            AddCheck(next_index, r);
            
            int base2, index;
            index = pre_base + a;
            base2 = bc_[index].base;
            bc_[next_index].base = base2;

            // 子リンクと兄弟リンクの遷移を行う
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

    // REBUILD関数に用いるためのGetChildrenRebuild
    std::vector<uint8_t> GetChildrenRebild(int r) {
        std::vector<uint8_t> row;
        int count = 0;
        if(tmp_bc_[r].base == kEmptyBase) {
            return row;
        }

        int next_node = tmp_bc_[r].base + tmp_bc_[r].child;
        row.emplace_back();
        row[count] = tmp_bc_[r].child;
        count++;

        while(true) {
            if(tmp_bc_[next_node].sibling == MaxUint8_t) {
                return  row;
            }
            row.emplace_back();
            row[count] = tmp_bc_[next_node].sibling;
            next_node = tmp_bc_[r].base + tmp_bc_[next_node].sibling;
            count++;
        }
    }

    // 使っている要素を削除して，未使用要素に連結する
    void Delete(int index) {
        //std::cout << "------- Delete ------" << std::endl;
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

            bc_[e_index].check = index;
            bc_[next_index].base = -1 * index;
            bc_[index].check = next_index;
            bc_[index].base = -1 * e_index;
            E_HEAD = index;
            
        }
    }

    // bc_[index].checkにvalを格納する
    // 未使用要素を再構築する
    void AddCheck(int index, int val) {
        //std::cout << "------- AddCheck ------" << std::endl;

        int e_index = E_HEAD;
        bool flag = true;
        
        if(!bc_[index].not_used) {
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
            
            bc_[prev_index].check = next_index;
            bc_[next_index].base = -1 * prev_index;
            flag = false;
        }
        if(flag) {
            int prev_index = -1 * bc_[index].base;
            int next_index = bc_[index].check;
            bc_[prev_index].check = next_index;
            bc_[next_index].base = -1 * prev_index;
        }
        bc_[index].check = val;

        // 要素をすべて使い切ってしまった時の処理
        if(!bc_[E_HEAD].not_used) {
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