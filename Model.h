#pragma once

#include <iostream>

#include <string>

#include <vector>

#include <array>

#include <algorithm>

// ==============================

// データテーブル

// ==============================

struct EnemyData

{

    const char* id;

    const char* name;

    int maxHp;

    int attack;

    int expReward;

    int goldReward;

    int def;


};

struct ItemData

{

    const char* id;

    const char* name;

    int heal;

    int price;

};

constexpr std::array<EnemyData, 3> ENEMY_TABLE{ {

    {"slime", "スライム", 18, 5, 8, 4,5},

    {"goblin", "ゴブリン", 30, 8, 15, 9,10},

    {"dragon", "ドラゴン", 75, 13, 50, 40,20}

} };

constexpr ItemData POTION{ "potion", "ポーション", 25, 10 };

// ==============================

// キャラクター

// ==============================

class Character

{

public:

    std::string name;

    int maxHp;

    int hp;

    int attack;

    int def;

    int mp;

    Character(const std::string& n, int maxHpValue, int attackValue,int defValue,int mpValue);

    bool IsAlive() const;

    int TakeDamage(int damage);

};

// ==============================

// プレイヤー

// ==============================

class Player : public Character

{

public:

    int level = 1;

    int exp = 0;

    int gold = 15;

    int potions = 2;

    explicit Player(const std::string& name);

    void GainExp(int amount);

    bool UsePotion();

};

// ==============================

// 敵

// ==============================

class Enemy : public Character

{

public:

    std::string id;

    int expReward;

    int goldReward;

    explicit Enemy(const EnemyData& data);

};

// ==============================

// ファクトリーパターン

// ==============================

class EnemyFactory

{

public:

    static Enemy Create(const std::string& id);

};

// ==============================

// オブジェクトプール

// ==============================

class MessagePool

{

private:

    std::vector<std::string> messages;

    size_t next = 0;

public:

    explicit MessagePool(size_t capacity = 8);

    void Add(const std::string& message);

    std::vector<std::string> GetRecent() const;

    void Clear();

};

// ==============================

// FSM

// ==============================

enum class GameState

{

    Title,

    Explore,

    Battle,

    Clear,

    GameOver,

    Exit

};

// ==============================

// ゲームモデル

// ==============================

class GameModel

{

private:

    GameState state = GameState::Title;

    Player player{ "勇者" };

    Enemy enemy = EnemyFactory::Create("slime");

    MessagePool messagePool;

    void Log(const std::string& message);

public:

    GameState GetState() const;

    const Player& GetPlayer() const;

    const Enemy& GetEnemy() const;

    std::vector<std::string> GetMessages() const;

    void StartNewGame();

    void StartBattle(const std::string& id);

    void PlayerAttack();

	void PlayerSkill();

    void PlayerUsePotion();

    void BuyPotion();

    void EscapeBattle();

    void ReturnToTitle();

    void SetState(GameState newState);

};

