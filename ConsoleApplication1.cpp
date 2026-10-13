#include "DxLib.h"
#include <algorithm>
#include <array>
#include <string>
#include <vector>

// ============================================================
// データテーブル
// ============================================================

struct EnemyData {
    const char* id;
    const char* name;
    int maxHp;
    int attack;
    int expReward;
    int goldReward;
};

constexpr std::array<EnemyData, 3> ENEMY_TABLE{ {
    {"slime",  "スライム", 18,  5,  8,  4},
    {"goblin", "ゴブリン", 30,  8, 15,  9},
    {"dragon", "ドラゴン", 75, 13, 50, 40},
} };

struct ItemData {
    const char* id;
    const char* name;
    int heal;
    int price;
};

constexpr ItemData POTION{ "potion", "ポーション", 25, 10 };

// ============================================================
// コンストラクタ：キャラクターの初期化
// ============================================================

class Character {
public:
    std::string name;
    int maxHp;
    int hp;
    int attack;

    Character(const std::string& n, int maxHpValue, int attackValue)
        : name(n), maxHp(maxHpValue), hp(maxHpValue), attack(attackValue) {
    }

    bool IsAlive() const { return hp > 0; }

    int TakeDamage(int damage) {
        const int actual = (std::max)(0, damage);
        hp = (std::max)(0, hp - actual);
        return actual;
    }
};

class Player : public Character {
public:
    int level = 1;
    int exp = 0;
    int gold = 15;
    int potions = 2;

    explicit Player(const std::string& playerName)
        : Character(playerName, 100, 12) {
    }

    void GainExp(int amount) {
        exp += amount;
        while (exp >= level * 20) {
            exp -= level * 20;
            ++level;
            maxHp += 10;
            attack += 3;
            hp = maxHp;
        }
    }

    bool UsePotion() {
        if (potions <= 0 || hp == maxHp) return false;
        hp = (std::min)(maxHp, hp + POTION.heal);
        --potions;
        return true;
    }
};

class Enemy : public Character {
public:
    std::string id;
    int expReward;
    int goldReward;

    explicit Enemy(const EnemyData& data)
        : Character(data.name, data.maxHp, data.attack),
        id(data.id), expReward(data.expReward), goldReward(data.goldReward) {
    }
};

// ============================================================
// ファクトリーパターン：敵の生成を集約
// ============================================================

class EnemyFactory {
public:
    static Enemy Create(const std::string& id) {
        for (const auto& data : ENEMY_TABLE) {
            if (id == data.id) return Enemy(data);
        }
        return Enemy(ENEMY_TABLE[0]); // 不明なIDはスライム
    }
};

// ============================================================
// オブジェクトプール：メッセージ文字列を再利用
// ============================================================

class MessagePool {
private:
    std::vector<std::string> messages;
    std::size_t next = 0;

public:
    explicit MessagePool(std::size_t capacity = 8)
        : messages(capacity) {
    }

    void Add(const std::string& message) {
        if (messages.empty()) return;
        messages[next] = message;
        next = (next + 1) % messages.size();
    }

    std::vector<std::string> GetRecent() const {
        std::vector<std::string> result;
        for (std::size_t i = 0; i < messages.size(); ++i) {
            const std::size_t index = (next + i) % messages.size();
            if (!messages[index].empty()) result.push_back(messages[index]);
        }
        return result;
    }

    void Clear() {
        for (auto& message : messages) message.clear();
        next = 0;
    }
};

// ============================================================
// キー入力：押した瞬間（キー押下エッジ）を検出
// 毎フレーム Update() を1回呼び、Pressed() で判定します。
// ============================================================

class KeyInput {
private:
    std::array<char, 256> current{};
    std::array<char, 256> previous{};

    KeyInput() = default;
    KeyInput(const KeyInput&) = delete;
    KeyInput& operator=(const KeyInput&) = delete;

public:
    static KeyInput& Instance() {
        static KeyInput instance;
        return instance;
    }

    void Update() {
        previous = current;
        GetHitKeyStateAll(current.data());
    }

    bool Pressed(int key) const {
        if (key < 0 || key >= static_cast<int>(current.size())) return false;
        return current[static_cast<std::size_t>(key)] != 0 &&
            previous[static_cast<std::size_t>(key)] == 0;
    }
};

// ============================================================
// 有限状態機械（FSM）
// ============================================================

enum class GameState {
    Title,
    Explore,
    Battle,
    Clear,
    GameOver,
    Exit
};

// ============================================================
// シングルトン：ゲーム管理クラス
// ============================================================

class GameManager {
private:
    GameState state = GameState::Title;
    Player player{ "勇者" };
    Enemy enemy = EnemyFactory::Create("slime");
    MessagePool messagePool;

    GameManager() = default;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    void Log(const std::string& text) {
        messagePool.Add(text);
    }

    void StartBattle(const std::string& enemyId) {
        enemy = EnemyFactory::Create(enemyId);
        messagePool.Clear();
        state = GameState::Battle;
        Log(enemy.name + "が現れた！");
    }

    void PlayerAttack() {
        const int damage = enemy.TakeDamage(player.attack);
        Log(player.name + "の攻撃！ " + enemy.name + "に" +
            std::to_string(damage) + "ダメージ！");
        if (!enemy.IsAlive()) {
            Log(enemy.name + "を倒した！");
            Log(std::to_string(enemy.expReward) + " EXP / " +
                std::to_string(enemy.goldReward) + " Gを獲得！");
            player.GainExp(enemy.expReward);
            player.gold += enemy.goldReward;

            if (enemy.id == "dragon") state = GameState::Clear;
            else state = GameState::Explore;
            return;
        }

        const int damageToPlayer = player.TakeDamage(enemy.attack);
        Log(enemy.name + "の攻撃！ " + std::to_string(damageToPlayer) +
            "ダメージを受けた！");
        if (!player.IsAlive()) state = GameState::GameOver;
    }

    void DrawTextAt(int x, int y, const std::string& text,
        unsigned int color = 0xFFFFFF) {
        DrawString(x, y, text.c_str(), color);
    }

    void DrawHeader(const char* title) {
        DrawBox(0, 0, 800, 76, 0x172238, TRUE);
        DrawString(28, 22, title, 0xFFFFFF);
        DrawLine(20, 76, 780, 76, 0x5C7599);
    }

    void DrawPlayerStatus() {
        DrawBox(24, 96, 350, 190, 0x202C42, TRUE);
        DrawString(42, 110, player.name.c_str(), 0xFFFFFF);
        DrawFormatString(42, 142, 0xFFFFFF, "Lv.%d", player.level);
        DrawFormatString(42, 168, 0xFFFFFF, "HP %d / %d", player.hp, player.maxHp);
        DrawFormatString(42, 194, 0xFFFFFF, "ATK %d   EXP %d", player.attack, player.exp);
        DrawFormatString(42, 220, 0xFFFFFF, "G %d   Potion %d", player.gold, player.potions);

        const int barWidth = 280;
        DrawBox(42, 250, 42 + barWidth, 266, 0x553333, TRUE);
        const int currentWidth = player.maxHp > 0
            ? barWidth * player.hp / player.maxHp : 0;
        DrawBox(42, 250, 42 + currentWidth, 266, 0x44BB66, TRUE);
    }

    void DrawMessages(int x, int y) {
        const auto recent = messagePool.GetRecent();
        int lineY = y;
        for (const auto& message : recent) {
            DrawTextAt(x, lineY, message);
            lineY += 25;
        }
    }

public:
    static GameManager& Instance() {
        static GameManager instance;
        return instance;
    }

    GameState GetState() const { return state; }

    void Update() {
        switch (state) {
        case GameState::Title:
            if (KeyInput::Instance().Pressed(KEY_INPUT_Z) || KeyInput::Instance().Pressed(KEY_INPUT_RETURN)) {
                player = Player("勇者");
                state = GameState::Explore;
            }
            if (KeyInput::Instance().Pressed(KEY_INPUT_X) || KeyInput::Instance().Pressed(KEY_INPUT_ESCAPE)) state = GameState::Exit;
            break;

        case GameState::Explore:
            if (KeyInput::Instance().Pressed(KEY_INPUT_1)) StartBattle("slime");
            else if (KeyInput::Instance().Pressed(KEY_INPUT_2)) StartBattle("goblin");
            else if (KeyInput::Instance().Pressed(KEY_INPUT_3)) {
                if (player.gold >= POTION.price) {
                    player.gold -= POTION.price;
                    ++player.potions;
                    Log("ポーションを購入した！");
                }
                else {
                    Log("ゴールドが足りない！");
                }
            }
            else if (KeyInput::Instance().Pressed(KEY_INPUT_4)) StartBattle("dragon");
            else if (KeyInput::Instance().Pressed(KEY_INPUT_ESCAPE)) state = GameState::Title;
            break;

        case GameState::Battle:
            if (KeyInput::Instance().Pressed(KEY_INPUT_1)) {
                PlayerAttack();
            }
            else if (KeyInput::Instance().Pressed(KEY_INPUT_2)) {
                if (player.UsePotion()) Log("ポーションを使って回復した！");
                else Log("ポーションがない、またはHPが満タンだ！");
                // 回復行動後、敵も攻撃する
                if (player.IsAlive()) {
                    const int damage = player.TakeDamage(enemy.attack);
                    Log(enemy.name + "の攻撃！ " +
                        std::to_string(damage) + "ダメージ！");
                    if (!player.IsAlive()) state = GameState::GameOver;
                }
            }
            else if (KeyInput::Instance().Pressed(KEY_INPUT_3) && enemy.id != "dragon") {
                Log("戦闘から逃げ出した！");
                state = GameState::Explore;
            }
            break;

        case GameState::Clear:
        case GameState::GameOver:
            if (KeyInput::Instance().Pressed(KEY_INPUT_Z) || KeyInput::Instance().Pressed(KEY_INPUT_RETURN)) {
                state = GameState::Title;
                messagePool.Clear();
            }
            if (KeyInput::Instance().Pressed(KEY_INPUT_X) || KeyInput::Instance().Pressed(KEY_INPUT_ESCAPE)) state = GameState::Exit;
            break;

        case GameState::Exit:
            break;
        }
    }

    void Draw() {
        SetDrawScreen(DX_SCREEN_BACK);
        ClearDrawScreen();
        DrawBox(0, 0, 800, 600, 0x101722, TRUE);

        switch (state) {
        case GameState::Title:
            DrawHeader("CONSOLE RPG - DxLib");
            DrawString(270, 190, "FANTASY RPG", 0x8ED6FF);
            DrawString(270, 250, "Z / ENTER : START", 0xFFFFFF);
            DrawString(270, 286, "X / ESC   : QUIT", 0xAAAAAA);
            DrawString(230, 390, "Explore, battle, and defeat the Dragon!", 0xD6DCE8);
            break;

        case GameState::Explore:
            DrawHeader("FIELD - 探索");
            DrawPlayerStatus();
            DrawString(420, 110, "どこへ行きますか？", 0x8ED6FF);
            DrawString(420, 155, "1 : 草原（スライム）", 0xFFFFFF);
            DrawString(420, 190, "2 : 洞窟（ゴブリン）", 0xFFFFFF);
            DrawString(420, 225, "3 : ポーション購入（10 G）", 0xFFFFFF);
            DrawString(420, 260, "4 : 最終決戦（ドラゴン）", 0xFF9B8F);
            DrawString(420, 295, "ESC : タイトルへ", 0xAAAAAA);
            DrawBox(24, 320, 776, 555, 0x202C42, TRUE);
            DrawString(42, 338, "ログ", 0x8ED6FF);
            DrawMessages(42, 372);
            break;

        case GameState::Battle:
            DrawHeader("BATTLE - 戦闘");
            DrawPlayerStatus();

            DrawBox(420, 100, 760, 286, 0x202C42, TRUE);
            DrawString(440, 120, enemy.name.c_str(), 0xFFB0A5);
            DrawFormatString(440, 154, 0xFFFFFF, "HP %d / %d", enemy.hp, enemy.maxHp);
            DrawBox(440, 190, 720, 208, 0x553333, TRUE);
            DrawBox(440, 190, 440 + 280 * enemy.hp / enemy.maxHp, 208, 0xD65B55, TRUE);

            // 素材不要の簡易キャラクター表示
            if (enemy.id == "slime") {
                DrawCircle(590, 250, 28, 0x55CC77, TRUE);
                DrawCircle(580, 244, 3, 0x101722, TRUE);
                DrawCircle(600, 244, 3, 0x101722, TRUE);
            }
            else if (enemy.id == "goblin") {
                DrawCircle(590, 236, 22, 0x8EBD62, TRUE);
                DrawTriangle(566, 226, 574, 200, 582, 226, 0x8EBD62, TRUE);
                DrawTriangle(598, 226, 606, 200, 614, 226, 0x8EBD62, TRUE);
                DrawCircle(583, 236, 3, 0x101722, TRUE);
                DrawCircle(598, 236, 3, 0x101722, TRUE);
            }
            else {
                DrawTriangle(550, 250, 575, 210, 590, 250, 0xC84E46, TRUE);
                DrawTriangle(590, 250, 620, 210, 640, 250, 0xC84E46, TRUE);
                DrawCircle(590, 245, 27, 0xC84E46, TRUE);
                DrawCircle(580, 240, 4, 0xFFE18A, TRUE);
                DrawCircle(600, 240, 4, 0xFFE18A, TRUE);
            }

            DrawBox(24, 320, 776, 555, 0x202C42, TRUE);
            DrawString(42, 338, "1 : 攻撃     2 : ポーション     3 : 逃げる（ボス以外）", 0x8ED6FF);
            DrawMessages(42, 375);
            break;

        case GameState::Clear:
            DrawHeader("GAME CLEAR");
            DrawString(270, 190, "ドラゴンを倒した！", 0xFFE18A);
            DrawString(270, 240, "世界に平和が戻った！", 0xFFFFFF);
            DrawFormatString(270, 290, 0xFFFFFF, "Lv.%d   Gold: %d", player.level, player.gold);
            DrawString(270, 360, "Z / ENTER : TITLE", 0xAAAAAA);
            DrawString(270, 395, "X / ESC   : QUIT", 0xAAAAAA);
            break;

        case GameState::GameOver:
            DrawHeader("GAME OVER");
            DrawString(300, 220, "勇者は力尽きた……", 0xFF7777);
            DrawString(270, 300, "Z / ENTER : TITLE", 0xFFFFFF);
            DrawString(270, 340, "X / ESC   : QUIT", 0xAAAAAA);
            break;

        case GameState::Exit:
            break;
        }

        ScreenFlip();
}
};

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    SetGraphMode(800, 600, 32);
    ChangeWindowMode(TRUE);
    SetMainWindowText("DxLib Simple RPG");
    if (DxLib_Init() == -1) return -1;

    SetDrawScreen(DX_SCREEN_BACK);
    GameManager& game = GameManager::Instance();

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー
    while (ProcessMessage() == 0 &&
        game.GetState() != GameState::Exit) {
        KeyInput::Instance().Update(); // 毎フレーム1回だけ更新
        game.Update();
        game.Draw();
        WaitTimer(16);
    }

    DxLib_End();
    return 0;
}
