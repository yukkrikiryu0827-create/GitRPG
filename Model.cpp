#include "Model.h"
// ==============================
// Character
// ==============================
Character::Character(
    const std::string& n,
    int maxHpValue,
    int attackValue)
    : name(n),
    maxHp(maxHpValue),
    hp(maxHpValue),
    attack(attackValue)
{
}
bool Character::IsAlive() const
{
    return hp > 0;
}
int Character::TakeDamage(int damage)
{
    int actual = (std::max)(0, damage);
    hp = (std::max)(0, hp - actual);
    return actual;
}
// ==============================
// Player
// ==============================
Player::Player(const std::string& name)
    : Character(name, 100, 12)
{
}
void Player::GainExp(int amount)
{
    exp += amount;
    while (exp >= level * 20)
    {
        exp -= level * 20;
        ++level;
        maxHp += 10;
        attack += 3;
        hp = maxHp;
       // std::cout << "レベルアップ！ Lv." << level << "\n";
    }
}
bool Player::UsePotion()
{
    if (potions <= 0 || hp == maxHp)
        return false;
    hp = (std::min)(maxHp, hp + POTION.heal);
    --potions;
    return true;
}
// ==============================
// Enemy
// ==============================
Enemy::Enemy(const EnemyData& data)
    : Character(data.name, data.maxHp, data.attack),
    id(data.id),
    expReward(data.expReward),
    goldReward(data.goldReward)
{
}
// ==============================
// EnemyFactory
// ==============================
Enemy EnemyFactory::Create(const std::string& id)
{
    for (const auto& data : ENEMY_TABLE)
    {
        if (id == data.id)
            return Enemy(data);
    }
    return Enemy(ENEMY_TABLE[0]);
}
// ==============================
// MessagePool
// ==============================
MessagePool::MessagePool(size_t capacity)
    : messages(capacity)
{
}
void MessagePool::Add(const std::string& message)
{
    if (messages.empty())
        return;
    messages[next] = message;
    next = (next + 1) % messages.size();
}
std::vector<std::string> MessagePool::GetRecent() const
{
    std::vector<std::string> result;
    for (size_t i = 0; i < messages.size(); ++i)
    {
        size_t index = (next + i) % messages.size();
        if (!messages[index].empty())
            result.push_back(messages[index]);
    }
    return result;
}
void MessagePool::Clear()
{
    for (auto& message : messages)
        message.clear();
    next = 0;
}
// ==============================
// GameModel
// ==============================
GameState GameModel::GetState() const
{
    return state;
}
const Player& GameModel::GetPlayer() const
{
    return player;
}
const Enemy& GameModel::GetEnemy() const
{
    return enemy;
}
std::vector<std::string> GameModel::GetMessages() const
{
    return messagePool.GetRecent();
}
void GameModel::Log(const std::string& message)
{
    messagePool.Add(message);
}
void GameModel::SetState(GameState newState)
{
    state = newState;
}
void GameModel::StartNewGame()
{
    player = Player("勇者");
    messagePool.Clear();
    state = GameState::Explore;
}
void GameModel::StartBattle(const std::string& id)
{
    enemy = EnemyFactory::Create(id);
    messagePool.Clear();
    state = GameState::Battle;
    Log(enemy.name + "が現れた！");
}
void GameModel::PlayerAttack()
{
    int damage = enemy.TakeDamage(player.attack);
    Log(player.name + "の攻撃！ " +
        enemy.name + "に" +
        std::to_string(damage) + "ダメージ！");
    if (!enemy.IsAlive())
    {
        Log(enemy.name + "を倒した！");
        Log(std::to_string(enemy.expReward) +
            " EXP / " +
            std::to_string(enemy.goldReward) +
            " Gを獲得！");
        player.GainExp(enemy.expReward);
        player.gold += enemy.goldReward;
        state = (enemy.id == "dragon")
            ? GameState::Clear
            : GameState::Explore;
        return;
    }
    int damageToPlayer = player.TakeDamage(enemy.attack);
    Log(enemy.name + "の攻撃！ " +
        std::to_string(damageToPlayer) +
        "ダメージを受けた！");
    if (!player.IsAlive())
        state = GameState::GameOver;
}
void GameModel::PlayerUsePotion()
{
    if (player.UsePotion())
        Log("ポーションを使って回復した！");
    else
        Log("ポーションがない、またはHPが満タンだ！");
    if (player.IsAlive())
    {
        int damage = player.TakeDamage(enemy.attack);
        Log(enemy.name + "の攻撃！ " +
            std::to_string(damage) + "ダメージ！");
        if (!player.IsAlive())
            state = GameState::GameOver;
    }
}
void GameModel::BuyPotion()
{
    if (player.gold >= POTION.price)
    {
        player.gold -= POTION.price;
        ++player.potions;
        Log("ポーションを購入した！");
    }
    else
    {
        Log("ゴールドが足りない！");
    }
}
void GameModel::EscapeBattle()
{
    if (enemy.id == "dragon")
        return;
    Log("戦闘から逃げ出した！");
    state = GameState::Explore;
}
void GameModel::ReturnToTitle()
{
    state = GameState::Title;
    messagePool.Clear();
}