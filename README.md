# Shooting Shapes 2D ゲーム

(English is Below)

**Shooting Shapes** は、プレイヤーがランダムに生成された図形を操作し、さまざまな敵の図形と戦う、スピード感のあるアクションゲームです。目標はできるだけ長く生き延びてポイントを稼ぎ、最高スコアを目指すことです。グラフィック描画や入力処理には **SFML** を使用しています。

**ビデオデモ**：[デモ動画を見る](https://www.youtube.com/watch?v=sr0TglMLr-w)

## 特徴

- ランダムな図形生成：プレイヤーの形はランダムに生成され、敵もランダムな位置に出現します。
- シューティング機能：プレイヤーは弾を撃って敵を倒すことができます。通常弾にはクールダウンがあり、すべての方向に弾を放つ強力なスーパームーブも使用可能です。
- サバイバル形式のゲームプレイ：敵を倒して生き延びることでスコアが加算されます。長く生き延びるほど高得点になります。
- 敵のAI：敵はウェーブ形式で出現し、回転しながらプレイヤーに向かって移動します。
- HUD 表示：スコア、残りライフ、スーパームーブの使用可否などのリアルタイム情報が表示されます。
- スーパームーブ：一定時間ごとに使用可能になる、全方向に弾を撃って敵を一掃する特別な能力です。

## 使用技術

- SFML：グラフィック描画、ウィンドウイベント処理、入力管理などに使用。
- C++：ゲームのロジックや機能を実装するためのメイン言語。

## ゲームプレイ

- プレイヤーの操作：矢印キーで図形を移動し、スペースキーで弾を撃ちます。
- 敵の出現：異なる形の敵がランダムに出現します。プレイヤーは衝突を避けながら敵を撃破する必要があります。
- ライフ：プレイヤーの初期ライフは3つです。敵と衝突するとライフを1つ失います。
- スーパームーブ：クールダウン後に使用可能になり、全方向に弾を発射して周囲の敵を一掃します。

## インストール方法とビルド手順

### 前提条件

SFML がインストールされ、プロジェクトに正しくリンクされている必要があります。

### リポジトリをクローン

```
git clone https://github.com/ludovicocuoghi/CPP_2D_Shooting_Shapes.git
cd CPP_2D_Shooting_Shapes
```

### プロジェクトのビルド

```
make
./bin/sfml_app
```

##　ゲームの実行

```
.bin/sfml_app
```

## ゲーム操作

| 操作キー                        | 内容                                |
|----------------------------------|-------------------------------------|
| WASD                             | プレイヤーの図形を移動              |
| 左クリック                       | 弾を発射                            |
| スーパームーブキー（使用可能時） | 全方向に弾を発射                    |

## ゲームの状態

- プレイ中：ゲームが進行中で、プレイヤーが図形を操作して敵と戦います。
- ゲームオーバー：ライフがすべて失われるとゲームオーバーになり、最終スコアと図形ごとのベストスコアが表示されます。

## コントリビューションについて

リポジトリをフォークして、改善や新機能の追加など自由に貢献してください。  
貢献する際は、Issue を立てるか、Pull Request を送信してください。


# Shooting Shapes 2D Game

**Shooting Shapes 2D** is a fast-paced action game where players control randomly generated shapes and battle against various enemy shapes. The goal is to survive as long as possible, earn points, and aim for the highest score. The game uses **SFML** for graphics rendering and input processing.

**Video Demo**: [Watch Demo Video](https://www.youtube.com/watch?v=sr0TglMLr-w)

## Features

- Random Shape Generation: Player shapes are randomly generated, and enemies appear at random positions.
- Shooting Mechanism: Players can shoot bullets to defeat enemies. Normal shots have a cooldown, and a powerful super move can also be used to shoot bullets in all directions.
- Survival-Style Gameplay: Score increases by defeating enemies and surviving. The longer you survive, the higher your score.
- Enemy AI: Enemies appear in waves and move toward the player while rotating.
- HUD Display: Real-time information such as score, remaining lives, and super move availability.
- Super Move: A special ability that becomes available periodically, shooting bullets in all directions to clear enemies.

## Technologies Used

- SFML: Used for graphics rendering, window event handling, and input management.
- C++: Main language for implementing game logic and features.

## Gameplay

- Player Controls: Move the shape with arrow keys and shoot bullets with the space key.
- Enemy Appearance: Enemies of different shapes appear randomly. Players need to avoid collisions while defeating enemies.
- Lives: Players start with 3 lives. One life is lost when colliding with an enemy.
- Super Move: Becomes available after a cooldown, firing bullets in all directions to clear surrounding enemies.

## Installation and Build Instructions

### Prerequisites

SFML must be installed and properly linked to the project.

### Clone the Repository

```
git clone https://github.com/ludovicocuoghi/CPP_2D_Shooting_Shapes.git
cd CPP_2D_Shooting_Shapes
```

### Build the Project

```
make clean
make
```

### Running the Game

```
.bin/sfml_app
```

## Game Controls

| Key                              | Action                                         |
|----------------------------------|------------------------------------------------|
| WASD                             | Move the player's shape                        |
| Left Mouse Click                 | Shoot bullets                                  |
| Supermove Key (if available)     | Fire bullets in all directions                |

## Game States

- Playing: Game is in progress, player controls the shape and battles enemies.
- Game Over: Occurs when all lives are lost, displaying final score and best scores for each shape.

## Contributing

Feel free to fork the repository and contribute with improvements or new features.  
When contributing, please open an Issue or submit a Pull Request.
