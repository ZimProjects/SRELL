■使用法

次のファイルを同じディレクトリに置き、srell.hppをincludeするだけです。
・srell.hpp
・srell_ucfdata2.hpp（case folding用データ）
・srell_updata2.hpp（Unicode property用データ）

■付属物
以下のディレクトリ内にあるものはおまけのようなものです。
SRELL側からは参照していませんので、削除してしまってもライブラリの動作に
影響はありません。

・misc
  簡単なテスト及びベンチマークプログラムのソースが入っています。

・single-header
  srell.hppの中にsrell_ucfdata2.hppとsrell_updata2.hppとを埋め込んで、こ
  れ単体で使用できるようにしたstandalone版が入っています。

・unicode
  最新のUnicodeデータからsrell_ucfdata2.hpp及びsrell_updata2.hppを作るた
  めのプログラムのソースが入っています。

