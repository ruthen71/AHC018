# AHC018

## Contest Link
- https://atcoder.jp/contests/ahc018

## やったこと

### 2023/02/23

#### インフラ

- レポジトリ作成
- サンプルプログラムを改造

#### ローカルテスタ導入

- [公式ドキュメント](https://www.rust-lang.org/ja/learn/get-started)
- [Qiita](https://qiita.com/evid/items/f81534518b30847a24d2)
- `cargo run --release --bin tester ../main < ./in/0000.txt > ./out/0000.txt`

- 2点の選び方と繋ぎ方を別々に考える
- デスクトップでテストを動かしたい
- インタラクティブチェック
    - AHC003(似てそう)
    - AHC007(似てそう)
    - AHC008
    - AHC015
    - AHC016
- [AHC003 2位解法](https://speakerdeck.com/yos1up/ahc003-2wei-jie-fa)みたいに分布をベイズ推定できないか?
    - $S_{i,j}$が正規分布じゃないので厳しいかも
- Perlin noiseの中身について
- 近くの結果を反映させる
- グリッドグラフにできる
- 頂点数 $2N^2$ 辺数 $N^2 + 4N^2$ (端はちょっと違う)
- A*
- RMQ(1点更新全体最小値)

### ToDo
- Googleの卵を落とす問題を試す

### 疑問
- 相対スコア評価の場合、どうやって解の比較をすれば良いんだろう
