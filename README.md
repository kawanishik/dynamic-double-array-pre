#   動的ダブル配列の作成

子リンク・兄弟リンクの追加<br>

# 変更点(子リンクと兄弟リンク以外で)
1. InsertSuffix内で，用いられていた "std::unordered_map<uint8_t, int> row" を以下のように変更し<br>
std::vector<uint8_t> row<br>
関数 "find_base" と関数 "ModifyAndInsertSuffix" で用いられていたunordered_mapも同様に<br>
  
2. tmp_childを削除して，代わりにtmp_arrを用いている<br>
tmp_arrは，構造体である<br>

3. 