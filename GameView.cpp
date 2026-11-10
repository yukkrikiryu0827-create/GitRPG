#include "GameView.h"

void GameView::DrawTitle()

{

    std::cout << "\n";

    std::cout << "==============================\n";

    std::cout << "       FANTASY RPG\n";

    std::cout << "==============================\n";

    std::cout << "1 : ゲームスタート\n";

    std::cout << "0 : 終了\n";

}

void GameView::DrawPlayerStatus(const Player& player)

{

    std::cout << "\n===== プレイヤー情報 =====\n";

    std::cout << "名前 : " << player.name << "\n";

    std::cout << "Lv   : " << player.level << "\n";

    std::cout << "HP   : " << player.hp
        << " / " << player.maxHp << "\n";

    std::cout << "ATK  : " << player.attack << "\n";

    std::cout << "EXP  : " << player.exp << "\n";

    std::cout << "GOLD : " << player.gold << "\n";

    std::cout << "Potion : " << player.potions << "\n";

    std::cout << "==========================\n";

}

void GameView::DrawMessages(const GameModel& model)

{

    std::cout << "\n===== ログ =====\n";

    for (const auto& message : model.GetMessages())

    {

        std::cout << message << "\n";

    }

    std::cout << "================\n";

}

void GameView::DrawExplore(const GameModel& model)

{

    std::cout << "\n===== 探索 =====\n";

    DrawPlayerStatus(model.GetPlayer());

    std::cout << "\nどこへ行きますか？\n";

    std::cout << "1 : 草原（スライム）\n";

    std::cout << "2 : 洞窟（ゴブリン）\n";

    std::cout << "3 : ポーション購入（10 G）\n";

    std::cout << "4 : 最終決戦（ドラゴン）\n";

    std::cout << "0 : タイトルへ\n";

    DrawMessages(model);

}

void GameView::DrawBattle(const GameModel& model)

{

    const Enemy& enemy = model.GetEnemy();

    std::cout << "\n===== BATTLE =====\n";

    DrawPlayerStatus(model.GetPlayer());

    std::cout << "\n敵 : " << enemy.name << "\n";

    std::cout << "HP : " << enemy.hp
        << " / " << enemy.maxHp << "\n";

    std::cout << "\n1 : 攻撃\n";

    std::cout << "2 : ポーション\n";

    if (enemy.id != "dragon")

        std::cout << "3 : 逃げる\n";

    DrawMessages(model);

}

void GameView::DrawClear(const GameModel& model)

{

    std::cout << "\n==============================\n";

    std::cout << "          GAME CLEAR\n";

    std::cout << "==============================\n";

    std::cout << "ドラゴンを倒した！\n";

    std::cout << "世界に平和が戻った！\n";

    std::cout << "Lv : " << model.GetPlayer().level << "\n";

    std::cout << "Gold : " << model.GetPlayer().gold << "\n";

    std::cout << "\n1 : タイトルへ\n";

    std::cout << "0 : 終了\n";

}

void GameView::DrawGameOver()

{

    std::cout << "\n==============================\n";

    std::cout << "          GAME OVER\n";

    std::cout << "==============================\n";

    std::cout << "勇者は力尽きた……\n";

    std::cout << "\n1 : タイトルへ\n";

    std::cout << "0 : 終了\n";

}

void GameView::Draw(const GameModel& model)

{

    switch (model.GetState())

    {

    case GameState::Title:

        DrawTitle();

        break;

    case GameState::Explore:

        DrawExplore(model);

        break;

    case GameState::Battle:

        DrawBattle(model);

        break;

    case GameState::Clear:

        DrawClear(model);

        break;

    case GameState::GameOver:

        DrawGameOver();

        break;

    case GameState::Exit:

        break;

    }

}
