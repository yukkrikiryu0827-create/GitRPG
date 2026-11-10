#pragma once

#include "Model.h"

#include "GameView.h"

class GameController

{

private:

    GameModel model;

    GameView view;

    int Input();

public:

    static GameController& Instance();

    void Run();

};

