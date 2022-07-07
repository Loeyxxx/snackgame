#ifndef GAME_H
#define GAME_H

#include "curses.h"
#include <string>
#include <vector>
#include <memory>


#include "snake.h"
#include "Timer.h""




class Game
{
public:
    Game();
    ~Game();

		void createInformationBoard();
    void renderInformationBoard() const;

    void createGameBoard();
    void renderGameBoard() const;

		void createInstructionBoard();
    void renderInstructionBoard() const;

		void loadLeadBoard();
    void updateLeadBoard();
    bool readLeaderBoard();
    bool updateLeaderBoard();
    bool writeLeaderBoard();
    void renderLeaderBoard() const;

		void renderBoards() const;

		void initializeGame();
    void runGame();
    void renderPoints() const;
    void renderDifficulty() const;

		void createRamdonFood();
    void renderFood() const;
    void renderSnake() const;
    void controlSnake() const;

		void startGame();
    bool renderRestartMenu() const;
    bool renderPauseMenu() const;
    void adjustDelay();

    //New Here
    void createRamdonPoison();
    void renderPoison() const;
    void appearPoison();
    //New here for Passage
    void createRamdonPassage();
    SnakeBody createNextDot(SnakeBody Dot);
    void renderPassage() const;
    void appearPassage();


private:
    // We need to have two windows
    // One is for game introduction
    // One is for game mWindows
    int mScreenWidth;
    int mScreenHeight;
    int mGameBoardWidth;
    int mGameBoardHeight;
    const int mInformationHeight = 6;
    const int mInstructionWidth = 18;
    std::vector<WINDOW *> mWindows;
    // Snake information
    const int mInitialSnakeLength = 2;
    const char mSnakeSymbol = '@';
    std::unique_ptr<Snake> mPtrSnake;
    // Food information
    SnakeBody mFood;
    const char mFoodSymbol = '#';
    //Poison information
    std::vector<SnakeBody> mPoison;
    const char mPoisonSymbol = 'X';
    int PoisonLevel;

    //Passage information
    std::vector<SnakeBody> mPassage;
    const char mPassageSymbol = '*';

    int mPoints = 0;
    int mDifficulty = 0;
    int mBaseDelay = 100;
    int mDelay;
    const std::string mRecordBoardFilePath = "record.dat";
    std::vector<int> mLeaderBoard;
    const int mNumLeaders = 3;
};

#endif
