#   動的ダブル配列の作成

TAILの追加<br>

## 変更点(関数について)
1. InsertTAIL関数の追加<br>
この関数は，TAILに追加するための関数<br>
base_changeがtrueの時は，TAIL上でもともと使用して部分に対して上書きしている<br>
falseの時は，TAILのサイズを拡大している<br>

2. InsertSuffix関数の変更<br>
過去 : 文字列ax内のすべての文字に対して，遷移を行うことができるように処理を行っていた<br>
現在 : 1文字目のみ "bc_" で遷移できるようにして，残りの文字列はInsertTAILに渡して，TAILに格納<br>

3. InsertSuffixAndTAIL関数の追加<br>
新しく追加する文字列がTAILのチェックにまで行き，遷移が失敗した時の処理をするための関数<br>
TAILと比較して，同じ部分(+1)を "bc_" 内で遷移できるようにし，残りをTAILに格納するようにしている<br>
注 : TAILの使わない部分を初期化する際に "MaxUint8_t-1"を使用している理由は，TAIL内の遷移で "MaxUint8_t" を使用しているためです<br>

4. contains関数の変更<br>
TAILの遷移ができるように変更した<br>

5. child-sibling と比較して，かなり構造が変化しているので，細かい変更が行われています 