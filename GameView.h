#pragma once

#include "Model.h"

class GameView

{

public:

    void Draw(const GameModel& model);

private:

    void DrawTitle();

    void DrawPlayerStatus(const Player& player);

    void DrawExplore(const GameModel& model);

    void DrawBattle(const GameModel& model);

    void DrawClear(const GameModel& model);

    void DrawGameOver();

    void DrawMessages(const GameModel& model);

};

