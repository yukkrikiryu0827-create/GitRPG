#include <limits>
#include "GameController.h"

GameController& GameController::Instance()

{

    static GameController instance;

    return instance;

}

int GameController::Input()


{

    int input;

    std::cout << "\n番号を入力してください：";

    if (!(std::cin >> input))

    {

        std::cin.clear();

        std::cin.ignore(

            (std::numeric_limits<std::streamsize>::max)(),

            '\n'

        );

        return -1;

    }

    return input;

}

void GameController::Run()

{

    while (model.GetState() != GameState::Exit)

    {

        // 画面表示

        view.Draw(model);

        // 入力受付

        int input = Input();

        // ゲーム進行

        switch (model.GetState())

        {

        case GameState::Title:

            if (input == 1)

                model.StartNewGame();

            else if (input == 0)

                model.SetState(GameState::Exit);

            break;

        case GameState::Explore:

            switch (input)

            {

            case 1:

                model.StartBattle("slime");

                break;

            case 2:

                model.StartBattle("goblin");

                break;

            case 3:

                model.BuyPotion();

                break;

            case 4:

                model.StartBattle("dragon");

                break;

            case 0:

                model.ReturnToTitle();

                break;

            }

            break;

        case GameState::Battle:

            switch (input)

            {

            case 1:

                model.PlayerAttack();

                break;

            case 2:

                model.PlayerUsePotion();

                break;

            case 3:

                model.EscapeBattle();

                break;

            case 4:
                model.PlayerSkill();

                break;

            }

            break;

        case GameState::Clear:

        case GameState::GameOver:

            if (input == 1)

                model.ReturnToTitle();

            else if (input == 0)

                model.SetState(GameState::Exit);

            break;

        case GameState::Exit:

            break;

        }

    }

    std::cout << "\nゲームを終了します。\n";

}
