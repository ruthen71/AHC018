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

- 2023-02-23 02:19:12 `power = 1000` にしたら大きすぎた
- 2023-02-23 02:55:20 `power = 10 * C`
- 2023-02-23 14:04:33 `power` の増え方が2べき
- 2023-02-23 15:18:33 クラスカル法っぽく繋ぐ(効果あり)
- 2点の選び方と繋ぎ方を別々に考える?
- デスクトップでテストを動かしたい
- インタラクティブ形式のAHCの過去問チェック
    - AHC003(似てそう), AHC007(似てそう), AHC008, AHC015, AHC016
- [AHC003 2位解法](https://speakerdeck.com/yos1up/ahc003-2wei-jie-fa)みたいに分布をベイズ推定できないか?
    - $S_{i,j}$が正規分布じゃないので厳しいかもしれない
- Perlin noiseの中身について
- 近くの結果を反映させる
- グリッドグラフにできる
- 頂点数 $2N^2$ 辺数 $N^2 + 4N^2$ (端はちょっと違う)
- A*
    - RMQ(1点更新全体最小値)

### 2023/02/24

- 2023-02-24 12:20:09 `power = 100` から `power = C * 5` に
- 進行方向3方向全部掘って一番傾きが小さいほうに進む
- S_allinfoの情報をdestructに使う
- 2023-02-24 17:31:37 A* っぽいもの
    - 微妙……
    - 例えば[wikipedia](https://ja.wikipedia.org/wiki/A*)の図でもわかるように、広めの範囲を掘り進めていく感じになってしまい、探索自体にコストがかかるのであまりよくなさそう
    - 自分では$h*(n)$としてまともなものが作れそうにない
- その過程で思いついたが、周辺の掘削結果から掘るマスのコストを予想するというのが効果的な気がする
- 2023-02-24 19:04:23 A* を爆破して作成(効果あり)
    - 掘り方はサンプルプログラムと同じ
- 2023-02-24 20:18:23 掘り方をシャッフル順列を使ったBFSでコストの小さいところに進むように変更(候補探索の分だけ掘る回数自体は増えている)

### 2023/02/25

- 2023-02-25 13:17:53 ゴールに絶対行くまで探索してしまうので、UnionFindを使って本来とは違うゴールについても終わりにするように変更

- あらかじめある程度掘っておくやつをやろうとしたがあまり上手くいかなかった

### 2023/02/26

- 2023-02-26 05:51:26 水源と家から十字方向にあらかじめほっておく奴
- ハイパーパラメータの調整が結構効果ありそうなのでOptunaを試してみる
- 2023-02-26 06:26:19 1回曲がる(右か左ランダムに)(微妙)
- 2023-02-26 11:39:01 ハイパーパラメータの最適化
- 2023-02-26 12:11:49 もう一度最適化(まだ効果あるが、過学習かも)
- 2023-02-26 12:46:33 対角線をなるべく進むように変更
- 2023-02-26 13:27:36 ハイパーパラメータの最適化(絶対スコアは増えているが、相対スコアは増えている?)
- 10回に1回くらい方向を変える判定を行う
- 2023-02-26 14:31:14 たまに方向転換する
- 2023-02-26 15:14:30 伸びた
    - マップは拡大率が変わったものだと考えられる
    - 方向転換する回数を拡大率とCによって変更、計算
- 2023-02-26 16:03:02 パラメータ変えただけ
- 2023-02-26 16:46:54 毎回水源と家をシャッフルする
- 2023-02-26 17:16:57 最近点対の計算でユークリッド距離を利用
- 2023-02-26 18:02:36 上記のやつをパラメータ最適化で20万点達成
- 2023-02-26 18:35:24 ラスト提出(もう少しパラメータ最適化したら逆に下がったが、手元のほうがテストケース数が多いことを信じたい)

### 疑問
- 相対スコア評価の場合、どうやって解の比較をすれば良かったのか
- やはり手元で大量に実行するしかない?
