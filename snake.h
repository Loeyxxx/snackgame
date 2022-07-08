#ifndef SNAKE_H
#define SNAKE_H

#include <vector>

enum class Direction
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
};

class SnakeBody
{
public:
    SnakeBody();
    SnakeBody(int x, int y);
    int getX() const;
    int getY() const;
    bool operator == (const SnakeBody& snakeBody);
private:
    int mX;
    int mY;
};

// Snake class should have no depency on the GUI library
class Snake
{
public:
    //Snake();
    Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength);
    // Set random seed
    void setRandomSeed();
    // Initialize snake
    void initializeSnake();
    // Checking API for generating random food
    bool isPartOfSnake(int x, int y);
    void senseFood(SnakeBody food);
    bool touchFood();
    // Check if the snake is dead
    bool hitWall();
    bool hitSelf();
    bool checkCollision();

    //Poison information
    void sensePoison(std::vector<SnakeBody> poison);
    bool touchPoison();
    void minusSnake();
    bool isTooSmall();
    //END

    //Passage information
    void sensePassage(std::vector<SnakeBody> passage);
    SnakeBody getHead();
    SnakeBody getNewHead();
    //END

    //Blood information;
    void senseBloodPassage(std::vector<SnakeBody> blood_passage);


    //Snake information
    int getSnakeLife();
    bool isDie();

    bool changeDirection(Direction newDirection);
    std::vector<SnakeBody>& getSnake();
    int getLength();
    SnakeBody createNewHead();
    bool moveFoward();

    Direction getDirection();

private:
    const int mGameBoardWidth;
    const int mGameBoardHeight;
    // Snake information
    const int mInitialSnakeLength;
    Direction mDirection;
    SnakeBody mFood;
    std::vector<SnakeBody> mPoison;
    std::vector<SnakeBody> mSnake;
    std::vector<SnakeBody> mPassgae;
    std::vector<SnakeBody> mBloodPassage;
};

#endif
