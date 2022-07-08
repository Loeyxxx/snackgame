#include <string>
#include <iostream>
#include <cmath>
#include <ctime>

// For terminal delay
#include <chrono>
#include <thread>


#include <fstream>
#include <algorithm>

#include "curses.h"
#include "game.h"
#include "Timer.h""

#include "winuser.h"
#define isKeyPressed(nVirtKey) (GetKeyState(nVirtKey) & (1<<(sizeof(SHORT)*8-1))) != 0


Game::Game()
{
    // Separate the screen to three windows
    this->mWindows.resize(3);
    initscr();
    // If there wasn't any key pressed don't wait for keypress
    nodelay(stdscr, true);
    // Turn on keypad control
    keypad(stdscr, true);
    // No echo for the key pressed
    noecho();
    // No cursor show
    curs_set(0);
    // Get screen and board parameters
    getmaxyx(stdscr, this->mScreenHeight, this->mScreenWidth);
    this->mGameBoardWidth = this->mScreenWidth - this->mInstructionWidth;
    this->mGameBoardHeight = this->mScreenHeight - this->mInformationHeight;

    this->createInformationBoard();
    this->createGameBoard();
    this->createInstructionBoard();

    // Initialize the leader board to be all zeros
    this->mLeaderBoard.assign(this->mNumLeaders, 0);
}

Game::~Game()
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        delwin(this->mWindows[i]);
    }
    endwin();
}

void Game::createInformationBoard()
{
    int startY = 0;
    int startX = 0;
    this->mWindows[0] = newwin(this->mInformationHeight, this->mScreenWidth, startY, startX);
}

void Game::renderInformationBoard() const
{
    mvwprintw(this->mWindows[0], 1, 1, "Welcome to The Snake Game!");
    mvwprintw(this->mWindows[0], 2, 1, "This is a mock version.");
    mvwprintw(this->mWindows[0], 3, 1, "Please fill in the blanks to make it work properly!!");
    mvwprintw(this->mWindows[0], 4, 1, "Implemented using C++ and libncurses library.");
    wrefresh(this->mWindows[0]);
}


void Game::createGameBoard()
{
    int startY = this->mInformationHeight;
    int startX = 0;
    this->mWindows[1] = newwin(this->mScreenHeight - this->mInformationHeight, this->mScreenWidth - this->mInstructionWidth, startY, startX);
}

void Game::renderGameBoard() const
{
    wrefresh(this->mWindows[1]);
}

void Game::createInstructionBoard()
{
    int startY = this->mInformationHeight;
    int startX = this->mScreenWidth - this->mInstructionWidth;
    this->mWindows[2] = newwin(this->mScreenHeight - this->mInformationHeight, this->mInstructionWidth, startY, startX);
}

void Game::renderInstructionBoard() const
{
    mvwprintw(this->mWindows[2], 1, 1, "Manual");

    mvwprintw(this->mWindows[2], 3, 1, "Up: W");
    mvwprintw(this->mWindows[2], 4, 1, "Down: S");
    mvwprintw(this->mWindows[2], 5, 1, "Left: A");
    mvwprintw(this->mWindows[2], 6, 1, "Right: D");

    mvwprintw(this->mWindows[2], 8, 1, "Difficulty");
    mvwprintw(this->mWindows[2], 11, 1, "Points");
    mvwprintw(this->mWindows[2], 14, 1, "Lifes");

    wrefresh(this->mWindows[2]);
}


void Game::renderLeaderBoard() const
{
    // If there is not too much space, skip rendering the leader board
    if (this->mScreenHeight - this->mInformationHeight - 14 - 2 < 3 * 2)
    {
        return;
    }
    mvwprintw(this->mWindows[2], 17, 1, "Leader Board");
    std::string pointString;
    std::string rank;
    for (int i = 0; i < std::min(this->mNumLeaders, this->mScreenHeight - this->mInformationHeight - 14 - 2); i ++)
    {
        pointString = std::to_string(this->mLeaderBoard[i]);
        rank = "#" + std::to_string(i + 1) + ":";
        mvwprintw(this->mWindows[2], 17 + (i + 1), 1, rank.c_str());
        mvwprintw(this->mWindows[2], 17 + (i + 1), 5, pointString.c_str());
    }
    wrefresh(this->mWindows[2]);
}

bool Game::renderRestartMenu() const
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"Restart", "Quit"};

    int index = 0;
    int offset = 4;
    mvwprintw(menu, 1, 1, "Your Final Score:");
    std::string pointString = std::to_string(this->AccumulatePoints());
    mvwprintw(menu, 2, 1, pointString.c_str());
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    if (index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool Game::renderPauseMenu() const
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"Continue", "Quit"};

    int index = 0;
    int offset = 4;
    mvwprintw(menu, 1, 1, "Your Score:");
    std::string pointString = std::to_string(this->AccumulatePoints());
    mvwprintw(menu, 2, 1, pointString.c_str());
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    if (index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Game::renderPoints() const
{
    std::string pointString = std::to_string(this->mPoints.back());
    mvwprintw(this->mWindows[2], 12, 1, pointString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::renderDifficulty() const
{
    std::string difficultyString = std::to_string(this->mDifficulty);
    mvwprintw(this->mWindows[2], 9, 1, difficultyString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::initializeGame()
{
    // allocate memory for a new snake
		this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));

    /* TODO
     * initialize the game points as zero
     * create a food at random place
     * make the snake aware of the food
     * other initializations
     */
     this->mSnakeLife = 3;
     this->mPoints.push_back(0);
     this->mDifficulty = 0;
     this->createRandomFood();
     this->mPtrSnake->senseFood(this->mFood);
     this->mDelay = this->mBaseDelay;

     //new
     this->PoisonLevel = 1;
     this->appearPoison();


}


void Game::createRandomFood()
{
/* TODO
 * create a food at random places
 * make sure that the food doesn't overlap with the snake.
 */
    std::vector<SnakeBody> availableGrids;
    for (int i = 1; i < this->mGameBoardHeight - 1; i ++)
    {
        for (int j = 1; j < this->mGameBoardWidth - 1; j ++)
        {
            if(!isValidDot(SnakeBody(j,i)))
            {
                continue;
            }
            else
            {
                availableGrids.push_back(SnakeBody(j, i));
            }
        }
    }

    // Randomly select a grid that is not occupied by the snake
    int random_idx = std::rand() % availableGrids.size();
    this->mFood = availableGrids[random_idx];
}

void Game::renderFood() const
{
    mvwaddch(this->mWindows[1], this->mFood.getY(), this->mFood.getX(), this->mFoodSymbol);
    wrefresh(this->mWindows[1]);
}

//Poison States
void Game::createRandomPoison()
{
    std::vector<SnakeBody>().swap(this->mPoison);
    std::vector<SnakeBody> availableGrids;
    for (int i = 1; i < this->mGameBoardHeight - 1; i ++)
    {
        for (int j = 1; j < this->mGameBoardWidth - 1; j ++)
        {
            if(!isValidDot(SnakeBody(j,i)))
            {
                continue;
            }
            else
            {
                availableGrids.push_back(SnakeBody(j, i));
            }
        }
    }

    for (int i=0;i<PoisonLevel;i++)
    {
        int random_idx = std::rand() % availableGrids.size();
        this->mPoison.push_back(availableGrids[random_idx]);
    }
}

void Game::renderPoison() const
{
    for(SnakeBody Poison : this->mPoison)
    {
        mvwaddch(this->mWindows[1],Poison.getY(), Poison.getX(), this->mPoisonSymbol);
    }
    wrefresh(this->mWindows[1]);
}

void Game::appearPoison()
{
    this->createRandomPoison();
    this->mPtrSnake->sensePoison(this->mPoison);
    this->renderPoison();
}
bool Game::isPartOfPoison(SnakeBody Head)
{
    for(SnakeBody Poison: this->mPoison)
    {
        if(Head == Poison)
            return true;
    }
    return false;
}
//Poison END

//Passage States
bool Game::isValidDot(SnakeBody Dot)
{
    if(isPartOfPassage(Dot))
        return false;
    if(isPartOfPoison(Dot))
        return false;
    if(isPartOfBloodPassage(Dot))
        return false;
    int DotX=Dot.getX(),DotY=Dot.getY();
    if(this->mPtrSnake->isPartOfSnake(DotX, DotY))
        return false;
    if (DotX < 1 || DotX >= this->mGameBoardHeight - 1 || DotY < 1 || DotY >= this->mGameBoardWidth -1)
        return false;
    return true;
}

void Game::createNextDoubleDot(SnakeBody Dot, std::vector<SnakeBody>& lines)
{
        int random_idx = std::rand() % 4;
        switch(random_idx)
        {
        case 0:
            {
                if(isValidDot(SnakeBody(Dot.getX()-1, Dot.getY())) && isValidDot(SnakeBody(Dot.getX()-2, Dot.getY())))
                {
                    lines.push_back(SnakeBody(Dot.getX()-1, Dot.getY()));
                    lines.push_back(SnakeBody(Dot.getX()-2, Dot.getY()));
                    break;
                }
                break;

            }
        case 1:
            {
                if(isValidDot(SnakeBody(Dot.getX()+1, Dot.getY())) && isValidDot(SnakeBody(Dot.getX()+2, Dot.getY())))
                {
                    lines.push_back(SnakeBody(Dot.getX()+1, Dot.getY()));
                    lines.push_back(SnakeBody(Dot.getX()+2, Dot.getY()));
                    break;
                }
                break;
            }
        case 2:
            {
                if(isValidDot(SnakeBody(Dot.getX(), Dot.getY()-1)) && isValidDot(SnakeBody(Dot.getX(), Dot.getY()-2)))
                {
                    lines.push_back(SnakeBody(Dot.getX(), Dot.getY()-1));
                    lines.push_back(SnakeBody(Dot.getX(), Dot.getY()-2));
                    break;
                }
                break;
            }
        case 3:
            {
                if(isValidDot(SnakeBody(Dot.getX(), Dot.getY()+1)) && isValidDot(SnakeBody(Dot.getX(), Dot.getY()+2)))
                {
                    lines.push_back(SnakeBody(Dot.getX(), Dot.getY()+1));
                    lines.push_back(SnakeBody(Dot.getX(), Dot.getY()+2));
                    break;
                }
                break;
            }
        }

}

void Game::createNextDot(SnakeBody Dot , std::vector<SnakeBody>& lines)
{
    int random_idx = std::rand() % 4;
        switch(random_idx)
        {
        case 0:
            {
                if(isValidDot(SnakeBody(Dot.getX()-1, Dot.getY())))
                {
                    lines.push_back(SnakeBody(Dot.getX()-1, Dot.getY()));
                }
                break;

            }
        case 1:
            {
                if(isValidDot(SnakeBody(Dot.getX()+1, Dot.getY())))
                {
                    lines.push_back(SnakeBody(Dot.getX()+1, Dot.getY()));
                }
                break;
            }
        case 2:
            {
                if(isValidDot(SnakeBody(Dot.getX(), Dot.getY()-1)))
                {
                    lines.push_back(SnakeBody(Dot.getX(), Dot.getY()-1));
                }
                break;
            }
        case 3:
            {
                if(isValidDot(SnakeBody(Dot.getX(), Dot.getY()+1)))
                {
                    lines.push_back(SnakeBody(Dot.getX(), Dot.getY()+1));
                }
                break;
            }
        }
}

int Game::judgeDirection(std::vector<SnakeBody> lines)
{
    if(lines.size() <= 1)
        return (std::rand() % 4) +1;
    //获得最后两个点
    SnakeBody LastDot = lines.back();
    SnakeBody Last2ndDot = lines.at(lines.size()-2);
    //为横线 左边没有点  返回1
    if(LastDot.getY() == Last2ndDot.getY() && LastDot.getX() < Last2ndDot.getX())
        return 1;
    //为横线 右边没有点 返回2
    if(LastDot.getY() == Last2ndDot.getY() && LastDot.getX() > Last2ndDot.getX())
        return 2;
    //为竖线 上边没有点 返回3
    if(LastDot.getX() == Last2ndDot.getX() && LastDot.getY() < Last2ndDot.getY())
        return 3;
    //为竖线 下边没有点 返回4
    if(LastDot.getX() == Last2ndDot.getX() && LastDot.getY() > Last2ndDot.getY())
        return 4;
}

void Game::createNextLine(std::vector<SnakeBody>& lines)
{
    SnakeBody Dot = lines.back();
    int Ndirection = judgeDirection(lines);
    switch(Ndirection)
    {
    case 1:
        {
            for(int i=1; i< (std::rand() % 5 )+3;i++)
            {
                if(isValidDot(SnakeBody(Dot.getX()-i, Dot.getY())))
                {
                lines.push_back(SnakeBody(Dot.getX()-i, Dot.getY()));
                }
                else break;
            }
            break;
        }
    case 2:
        {
                for(int i=1; i<(std::rand() % 5 )+3;i++)
                {
                    if(isValidDot(SnakeBody(Dot.getX()+i, Dot.getY())))
                    {
                    lines.push_back(SnakeBody(Dot.getX()+i, Dot.getY()));
                    }
                    else break;
                }
                break;
        }
    case 3:
        {
            for(int i=1; i<(std::rand() % 5 )+3;i++)
                {
                    if(isValidDot(SnakeBody(Dot.getX(), Dot.getY()-i)))
                    {
                    lines.push_back(SnakeBody(Dot.getX(), Dot.getY()-i));
                    }
                    else break;
                }
            break;
        }
    case 4:
        {
            for(int i=1; i<(std::rand() % 5 )+3;i++)
                {
                    if(isValidDot(SnakeBody(Dot.getX(), Dot.getY()+i)))
                    {
                    lines.push_back(SnakeBody(Dot.getX(), Dot.getY()+i));
                    }
                    else break;
                }
            break;
        }

    }
}

void Game::createTurningDot(std::vector<SnakeBody>& lines)
{
    SnakeBody LastDot = lines.back();
    int nDirection = judgeDirection(lines);
    if(nDirection == 1 || nDirection == 2)
    {
        int random_idx = std::rand() % 2;
        switch(random_idx)
        {
            case 0:
            {
                if(isValidDot(SnakeBody(LastDot.getX(),LastDot.getY()-1)))
                {
                    lines.push_back(SnakeBody(LastDot.getX(),LastDot.getY()-1));
                    break;
                }
                break;
            }
            case 1:
            {
                if(isValidDot(SnakeBody(LastDot.getX(),LastDot.getY()+1)))
                {
                    lines.push_back(SnakeBody(LastDot.getX(),LastDot.getY()+1));
                    break;
                }
                break;
            }
        }
    }
    if(nDirection == 3 || nDirection == 4)
    {
        int random_idx = std::rand() % 2;
        switch(random_idx)
        {
            case 0:
            {
                if(isValidDot(SnakeBody(LastDot.getX()-1,LastDot.getY())))
                {
                    lines.push_back(SnakeBody(LastDot.getX()-1,LastDot.getY()));
                    break;
                }
                break;
            }
            case 1:
            {
                if(isValidDot(SnakeBody(LastDot.getX()+1,LastDot.getY())))
                {
                    lines.push_back(SnakeBody(LastDot.getX()+1,LastDot.getY()));
                    break;
                }
                break;
            }
        }
    }
}


/*void Game::createHardPassage()
{
    std::vector<SnakeBody>().swap(this->mPassage);
    std::vector<SnakeBody> availableGrids;
    for (int i = 1; i < this->mGameBoardHeight - 1; i ++)
    {
        for (int j = 1; j < this->mGameBoardWidth - 1; j ++)
        {
            if(this->mPtrSnake->isPartOfSnake(j, i))
            {
                continue;
            }
            else
            {
                availableGrids.push_back(SnakeBody(j, i));
            }
        }
    }
    int random_idx = std::rand() % availableGrids.size();
    this->mPassage.push_back(availableGrids[random_idx]);
    for (int i=0;i < 10 ; i++)
    {
        this->createNextDoubleDot(this->mPassage.back());
    }
    if(this->mPassage.size() < 2 )
        std::vector<SnakeBody>().swap(this->mPassage);
}
*/

void Game::createRandomPassage()
{
    std::vector<SnakeBody>().swap(this->mPassage);
    std::vector<SnakeBody> availableGrids;
    for (int i = 1; i < this->mGameBoardHeight - 1; i ++)
    {
        for (int j = 1; j < this->mGameBoardWidth - 1; j ++)
        {
            if(!isValidDot(SnakeBody(j,i)))
            {
                continue;
            }
            else
            {
                availableGrids.push_back(SnakeBody(j, i));
            }
        }
    }
    int random_idx = std::rand() % availableGrids.size();
    this->mPassage.push_back(availableGrids[random_idx]);
    for (int i=0;i < (std::rand() %5) +2 ; i++)
    {
        this->createNextLine(this->mPassage);
        this->createTurningDot(this->mPassage);

    }
    if(this->mPassage.size() < 3 )
        std::vector<SnakeBody>().swap(this->mPassage);

}

void Game::renderPassage() const
{
    for(SnakeBody Passage : this->mPassage)
    {
        if(!this->mPtrSnake->isPartOfSnake(Passage.getX(),Passage.getY()))
            mvwaddch(this->mWindows[1],Passage.getY(), Passage.getX(), this->mPassageSymbol);
    }
    wrefresh(this->mWindows[1]);
}

void Game::appearPassage()
{
    this->createRandomPassage();
    this->mPtrSnake->sensePassage(this->mPassage);
    this->renderPassage();
}

bool Game::isPartOfPassage(SnakeBody Head)
{
    for(SnakeBody Passage: this->mPassage)
    {
        if(Head == Passage)
            return true;
    }
    return false;
}

bool Game::is_inVector(std::vector<SnakeBody> walkThrough, SnakeBody Position)
{
    for(SnakeBody position : walkThrough)
    {
        if(position == Position)
            return true;
    }
    return false;
}

//Passage END

//Blood States
bool Game::isPartOfBloodPassage(SnakeBody Head)
{
    for(SnakeBody Blood: this->mBloodPassage)
    {
        if(Head == Blood)
            return true;
    }
    return false;
}

void Game::createRandomBloodPassage()
{
    std::vector<SnakeBody>().swap(this->mBloodPassage);
    std::vector<SnakeBody> availableGrids;
    for (int i = 1; i < this->mGameBoardHeight - 1; i ++)
    {
        for (int j = 1; j < this->mGameBoardWidth - 1; j ++)
        {
            if(!isValidDot(SnakeBody(j,i)))
            {
                continue;
            }
            else
            {
                availableGrids.push_back(SnakeBody(j, i));
            }
        }
    }
    int random_idx = std::rand() % availableGrids.size();
    this->mBloodPassage.push_back(availableGrids[random_idx]);
    for (int i=0;i < 5 ; i++)
    {
        this->createNextDoubleDot(this->mBloodPassage.back(),this->mBloodPassage);
    }
    if(this->mPassage.size() < 5 )
        std::vector<SnakeBody>().swap(this->mBloodPassage);
}

void Game::renderBloodPassage() const
{
    for(SnakeBody BloodPassage : this->mBloodPassage)
    {
        if(BloodPassage == this->mBloodPassage.back())
        {
            mvwaddch(this->mWindows[1],BloodPassage.getY(), BloodPassage.getX(), this->mBloodSymbol);
            continue;
        }
        if(!this->mPtrSnake->isPartOfSnake(BloodPassage.getX(),BloodPassage.getY()))
            mvwaddch(this->mWindows[1],BloodPassage.getY(), BloodPassage.getX(), this->mBloodPassageSymbol);
    }
    wrefresh(this->mWindows[1]);
}

void Game::appearBloodPassage()
{
    this->createRandomBloodPassage();
    this->mPtrSnake->senseBloodPassage(this->mBloodPassage);
    this->renderBloodPassage();
}

//Blood END

void Game::renderSnake() const
{
    int snakeLength = this->mPtrSnake->getLength();
    std::vector<SnakeBody>& snake = this->mPtrSnake->getSnake();
    for (int i = 0; i < snakeLength; i ++)
    {
        mvwaddch(this->mWindows[1], snake[i].getY(), snake[i].getX(), this->mSnakeSymbol);
    }
    wrefresh(this->mWindows[1]);
}

void Game::controlSnake() const
{
    int key;
    key = getch();
    switch(key)
    {
        case 'W':
        case 'w':
        case KEY_UP:
        {
				    // TODO change the direction of the snake.
            this->mPtrSnake->changeDirection(Direction::Up);

            break;
        }
        case 'S':
        case 's':
        case KEY_DOWN:
        {
				    // TODO change the direction of the snake.
            this->mPtrSnake->changeDirection(Direction::Down);


            break;
        }
        case 'A':
        case 'a':
        case KEY_LEFT:
        {
				    // TODO change the direction of the snake.

            this->mPtrSnake->changeDirection(Direction::Left);


            break;
        }
        case 'D':
        case 'd':
        case KEY_RIGHT:
        {
				    // TODO change the direction of the snake.
            this->mPtrSnake->changeDirection(Direction::Right);



            break;
        }
        case ' ':
        case 8:
        {
            bool choice = this->renderPauseMenu();
            if (choice == false)
            {
                exit(0);
            }
        }
        default:
        {
            break;
        }
    }
}

void Game::renderBoards() const
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        werase(this->mWindows[i]);
    }
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard();
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        box(this->mWindows[i], 0, 0);
        wrefresh(this->mWindows[i]);
    }
    this->renderLeaderBoard();
}


void Game::adjustDelay()
{
    this->mDifficulty = this->mPoints.back() / 5;
    if (this->mPoints.back() % 5 == 0)
    {
        this->mDelay = this->mBaseDelay * pow(0.75, this->mDifficulty);
    }

}


void Game::runGame()
{
    bool moveSuccess;
    int key;
    //time information
    Timer playTimer;
    time_t playBeforeTick,playNowTick;
    playTimer.startTimer();

    //Passage information
    std::vector<SnakeBody> PassageWalkThrough;
    Timer PassageTimer;
    time_t PassageTick;

    //Blood information
    std::vector<SnakeBody> BloodWalkThrough;
    Timer BloodTimer;
    time_t BloodTick;

    while (true)
    {
				/* TODO
				 * this is the main control loop of the game.
				 * it keeps running a while loop, and does the following things:
				 * 	1. process your keyboard input
				 * 	2. clear the window
				 * 	3. move the current snake forward
				 * 	4. check if the snake has eaten the food after movement
				 * 	5. check if the snake dies after the movement
				 * 	6. make corresponding steps for the ``if conditions'' in 3 and 4.
				 *   7. render the position of the food and snake in the new frame of window.
				 *   8. update other game states and refresh the window
                 */
        //New Here
        playTimer.updateTime();
        playNowTick = playTimer.getTick();

        //
        if(playNowTick != playBeforeTick && playNowTick % 5 == 0)
        {
            this->appearPoison();
        }
        //↑
        this->controlSnake();
        werase(this->mWindows[1]);
        box(this->mWindows[1],0,0);

        bool eatFood = this->mPtrSnake->moveFoward();
        if(this->mPtrSnake->checkCollision() == true)
        {
            this->mSnakeLife--;
            if(this->isDie())
                break;
            this->RestartGame();
            playTimer.startTimer();
            continue;
        }
        //New Here
        if(this->mPtrSnake->touchPoison()== true)
        {
            this->mPtrSnake->minusSnake();
            int nowPoint = this->mPoints.back();
            this->mPoints.pop_back();
            nowPoint--;
            this->mPoints.push_back(nowPoint);
            if(this->mPtrSnake->isTooSmall())
            {
                this->mSnakeLife--;
                if(this->isDie())
                    break;
                this->RestartGame();
                playTimer.startTimer();
                continue;
            }
            this->adjustDelay();
            this->appearPoison();
        }

        //New Passage
        if(playNowTick != playBeforeTick && playNowTick % 7 == 0 && std::rand() % 2 == 0)
        {
            if(this->mPassage.size() == 0)
            {
                this->appearPassage();
                PassageTimer.startTimer();
                PassageWalkThrough = this->mPassage;
            }
        }

        if(this->mPassage.size()>0)
        {
            PassageTimer.updateTime();
            PassageTick = PassageTimer.getTick();
            if(!isPartOfPassage(this->mPtrSnake->getHead()) && PassageWalkThrough.size() != this->mPassage.size())
            {
                PassageWalkThrough = this->mPassage;
            }
            if(isPartOfPassage(this->mPtrSnake->getHead()))
            {
                SnakeBody Position = this->mPtrSnake->getHead();
                if(!is_inVector(PassageWalkThrough, Position))
                {
                    PassageWalkThrough = this->mPassage;
                }
                else
                {
                    std::vector<SnakeBody>::iterator pos;
                    pos = find(PassageWalkThrough.begin(), PassageWalkThrough.end(), Position);
                    if(pos != PassageWalkThrough.end())
                        PassageWalkThrough.erase(pos);
                    else PassageWalkThrough.pop_back();
                }
                if(PassageWalkThrough.size() == 0)
                {
                    int nowPoint = this->mPoints.back();
                    this->mPoints.pop_back();
                    nowPoint += (int)(this->mPassage.size());
                    this->mPoints.push_back(nowPoint);
                    std::vector<SnakeBody>().swap(this->mPassage);
                    std::vector<SnakeBody>().swap(PassageWalkThrough);
                    this->adjustDelay();
                }
            }
            if(PassageTick >= 15)
                {
                    std::vector<SnakeBody>().swap(this->mPassage);
                }
        }

        //Passage END

        //New BloodPassage
        if(playNowTick != playBeforeTick && playNowTick % 7 == 0 && std::rand() % 2 == 0)
        {
            if(this->mBloodPassage.size() == 0)
            {
                this->appearBloodPassage();
                BloodTimer.startTimer();
                BloodWalkThrough = this->mBloodPassage;
            }
        }

        if(this->mBloodPassage.size()>0)
        {
            BloodTimer.updateTime();
            BloodTick = BloodTimer.getTick();
            if(!isPartOfBloodPassage(this->mPtrSnake->getHead()) && BloodWalkThrough.size() != this->mBloodPassage.size())
            {
                BloodWalkThrough = this->mBloodPassage;
            }
            if(isPartOfBloodPassage(this->mPtrSnake->getHead()))
            {
                //还没写必须从头开始
                SnakeBody Position = this->mPtrSnake->getHead();
                if(!is_inVector(BloodWalkThrough, Position))
                {
                    BloodWalkThrough = this->mBloodPassage;
                }
                else
                {
                    std::vector<SnakeBody>::iterator pos;
                    pos = find(BloodWalkThrough.begin(), BloodWalkThrough.end(), Position);
                    if(pos != BloodWalkThrough.end())
                        BloodWalkThrough.erase(pos);
                    else BloodWalkThrough.pop_back();
                }
                if(BloodWalkThrough.size() == 0)
                {
                    this->mSnakeLife++;
                    std::vector<SnakeBody>().swap(this->mBloodPassage);
                    std::vector<SnakeBody>().swap(BloodWalkThrough);
                }
            }
            if(BloodTick >= 15)
                {
                    std::vector<SnakeBody>().swap(this->mBloodPassage);
                }
        }
        //Blood END

        //END
        this->renderSnake();
        if(eatFood == true)
        {
            int nowPoint = this->mPoints.back();
            this->mPoints.pop_back();
            nowPoint++;
            this->mPoints.push_back(nowPoint);
            this->createRandomFood();
            this->mPtrSnake->senseFood(this->mFood);
            this->adjustDelay();
        }
        this->renderFood();
        this->renderDifficulty();
        this->renderPoints();
        this->renderLife();
        this->renderPoison();
        this->renderPassage();
        this->renderBloodPassage();






        playTimer.updateTime();
        playBeforeTick = playTimer.getTick();
				std::this_thread::sleep_for(std::chrono::milliseconds(this->mDelay));
        refresh();
    }
}

void Game::startGame()
{
    refresh();
    bool choice;
    while (true)
    {
        this->readLeaderBoard();
        this->renderBoards();
        this->initializeGame();
        this->runGame();
        this->updateLeaderBoard();
        this->writeLeaderBoard();
        choice = this->renderRestartMenu();
        if (choice == false)
        {
            break;
        }
    }
}

// https://en.cppreference.com/w/cpp/io/basic_fstream
bool Game::readLeaderBoard()
{
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.in);
    if (!fhand.is_open())
    {
        return false;
    }
    int temp;
    int i = 0;
    while ((!fhand.eof()) && (i < mNumLeaders))
    {
        fhand.read(reinterpret_cast<char*>(&temp), sizeof(temp));
        this->mLeaderBoard[i] = temp;
        i ++;
    }
    fhand.close();
    return true;
}

bool Game::updateLeaderBoard()
{
    bool updated = false;
    int newScore = this->AccumulatePoints();
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        if (this->mLeaderBoard[i] >= this->AccumulatePoints())
        {
            continue;
        }
        int oldScore = this->mLeaderBoard[i];
        this->mLeaderBoard[i] = newScore;
        newScore = oldScore;
        updated = true;
    }
    return updated;
}

bool Game::writeLeaderBoard()
{
    // trunc: clear the data file
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.trunc | fhand.out);
    if (!fhand.is_open())
    {
        return false;
    }
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        fhand.write(reinterpret_cast<char*>(&this->mLeaderBoard[i]), sizeof(this->mLeaderBoard[i]));;
    }
    fhand.close();
    return true;
}

//SnakeLife State
void Game::renderLife() const
{
    std::string lifeString = std::to_string(this->mSnakeLife);
    mvwprintw(this->mWindows[2], 15, 1, lifeString.c_str());
    wrefresh(this->mWindows[2]);
}

void Game::RestartGame()
{
    //主动释放内存
    this->mPtrSnake = nullptr;
		this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));

     this->mPoints.push_back(0);
     this->mDifficulty = 0;
     this->createRandomFood();
     this->mPtrSnake->senseFood(this->mFood);
     this->mDelay = this->mBaseDelay;

     //new
     this->PoisonLevel = 1;
     this->appearPoison();
     werase(this->mWindows[2]);
     this->readLeaderBoard();
     this->renderBoards();
}

bool Game::isDie()
{
    if(this->mSnakeLife == 0)
        return true;
    return false;
}

int Game::AccumulatePoints() const
{
    int ans=0;
    for(int point: this->mPoints)
    {
        ans += point;
    }
    return ans;
}







