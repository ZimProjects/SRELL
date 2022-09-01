■同梱物について

  1. ucfdataout2.cpp
  2. updataout2.cpp

----
1. ucfdataout2.cpp

  srell_ucfdata2.hppの最新版を作成するプログラムのソースファイルです。SRELLの
2.5以降はcase-insensitiveな（大文字小文字の違いを無視した）照合を行うために、
このsrell_ucfdata2.hppを必要とします。

  ucfdataout2は、Unicode Consortiumより提供されているCaseFolding.txtというテキ
ストデータからsrell_ucfdata2.hppを自動生成します。

  +---------------------------------------------------------------------------
  | CaseFolding.txtとは
  |
  |   Case-insensitiveな照合を行う際には、大文字小文字の違いを吸収するために
  | "case-folding" と呼ばれる処理が行われます。Unicode規格に基づいた
  | case-foldingを行うために、Unicode Consortiumから提供されているのが
  | CaseFolding.txtです。
  |
  |   このデータファイルはUnicode規格がアップデートされるとそれに合わせて
  | アップデートされる可能性があります。
  |
  +---------------------------------------------------------------------------

  1-1. 使用方法

    1) ucfdataout2.cppをコンパイルします。
    2) 最新版のCaseFolding.txtを次のURLより取得します。
       http://www.unicode.org/Public/UNIDATA/CaseFolding.txt ,
    3) CaseFolding.txtと、1)で作成したバイナリとを同じフォルダに置いて
       バイナリを実行します。
    4) srell_ucfdata2.hppが生成されますので、それをSRELLの置かれているディレク
       トリへと移動させます。

  1-2. 互換性

    srell_ucfdata2.hppは、SRELL 2.401までが利用していたsrell_updata.hppと互換
    性がありません。

----
2. updataout2.cpp

  srell_updata2.hppの最新版を作成するプログラムのソースファイルです。SRELLは
Unicode property escapes（\p{...} と \P{...}）を含む正規表現と文字列との照合
を行うために、このsrell_updata2.hppを必要とします。

  updataout2は、Unicode Consortiumより提供されている次のテキストデータから
srell_updata2.hppを自動生成します。

  ・DerivedCoreProperties.txt
  ・DerivedNormalizationProps.txt
  ・emoji-data.txt
  ・PropList.txt
  ・ScriptExtensions.txt
  ・Scripts.txt
  ・UnicodeData.txt
  ・emoji-sequences.txt
  ・emoji-zwj-sequences.txt

  先述のCaseFolding.txt同様、これらのテキストデータファイルもUnicode規格が
アップデートされるとそれに合わせてアップデートされる可能性があります。

  2-1. 使用方法

    1) updataout2.cppをコンパイルします。
    2) 前記テキストファイルの最新版を次のURLより取得します。
       a. emoji-data.txt: http://www.unicode.org/Public/UNIDATA/emoji/
       b. emoji-sequences.txt と emoji-zwj-sequences.txt:
          http://www.unicode.org/Public/emoji/(ヴァージョン番号)/
       c. その他: http://www.unicode.org/Public/UNIDATA/
    3) これらのテキストファイルと、1)で作成したバイナリとを同じフォルダに
       置いてバイナリを実行します。
    4) srell_updata2.hppが生成されますので、それをSRELLの置かれているディレク
       トリへと移動させます。

  2-2. 互換性

    srell_updata2.hppは、SRELL 3.010までが利用していたsrell_updata.hppと互換性
    がありません。

