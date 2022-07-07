#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "snake.h"


SnakeBody::SnakeBody()
{
}


SnakeBody::SnakeBody(int x, int y): mX(x), mY(y)
{
}

int SnakeBody::getX() const
{
    return mX;
}

int SnakeBody::getY() const
{
    return mY;
}

bool SnakeBody::operator == (const SnakeBody& snakeBody)
{
		// TODO overload the == operator for SnakeBody comparision.

    if(this->mX == snakeBody.getX() && this->mY == snakeBody.getY())
        return true;


    return false;
}

Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength): mGameBoardWidth(gameBoardWidth), mGameBoardHeight(gameBoardHeight), mInitialSnakeLength(initialSnakeLength)
{
    this->initializeSnake();
    this->setRandomSeed();
}

void Snake::setRandomSeed()
{
    // use current time as seed for random generator
    std::srand(std::time(nullptr));
}

void Snake::initializeSnake()
{
    // Instead of using a random initialization algorithm
    // We always put the snake at the center of the game mWindows
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    for (int i = 0; i < this->mInitialSnakeLength; i ++)
    {
        this->mSnake.push_back(SnakeBody(centerX, centerY + i));
    }
    this->mDirection = Direction::Up;
}

bool Snake::isPartOfSnake(int x, int y)
{
		// TODO check if a given point with axis x, y is on the body of the snake.
    for(int i=0;i<int(this->mSnake.size());i++)
    {
        if(this->mSnake[i].getX() == x && this->mSnake[i].getY() == y)
            return true;
    }




    return false;
}

/*Snake Length-1
*/
void Snake::minusSnake()
{
    this->mSnake.pop_back();
}
/*
 * Assumption:
 * Only the head would hit wall.
 */
bool Snake::hitWall()
{
		// TODO check if the snake has hit the wall
    if(this->mSnake[0].getX() <= 0 || this->mSnake[0].getX() >= mGameBoardWidth-1)
        return true;
    if(this->mSnake[0].getY() <= 0 || this->mSnake[0].getY() >= mGameBoardHeight-1)
        return true;





    return false;
}

/*
 * The snake head is overlapping with its body
 */
bool Snake::hitSelf()
{
		// TODO check if the snake has hit itself.
    SnakeBody Head = this->mSnake[0];
    for(int i=1;i<int(this->mSnake.size());i++)
    {
        if(this->mSnake[i] == Head)
            return true;
    }



    return false;
}


bool Snake::touchFood()
{
    SnakeBody newHead = this->createNewHead();
    if (this->mFood == newHead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Snake::senseFood(SnakeBody food)
{
    this->mFood = food;
}

//Poison Item
void Snake::sensePoison(std::vector<SnakeBody> poison)
{
    this->mPoison = poison;
}

bool Snake::touchPoison()
{
    SnakeBody Head = this->mSnake[0];
    for(SnakeBody Poison : this->mPoison)
    {
        if(Head == Poison)
        {
            return true;
        }
    }
    return false;
}

//END

//Passage Item
void Snake::sensePassage(std::vector<SnakeBody> passage)
{
    this->mPassgae = passage;
}

bool Snake::throughPassage()
{
    for(SnakeBody Passage : this->mPassgae)
    {
        if(!this->isPartOfSnake(Passage.getX(),Passage.getY()))
            return false;
    }
    return true;
}

//SnakeBody Snake::


std::vector<SnakeBody>& Snake::getSnake()
{
    return this->mSnake;
}

bool Snake::changeDirection(Direction newDirection)
{
    switch (newDirection)
    {
        case Direction::Up:
        {
						// what you need to do when the current direction of the snake is Up
						// and the user inputs a new direction?  TODO
            if(this->mDirection == Direction::Down || this->mDirection == Direction::Up) return false;
            else
            {
                this->mDirection = newDirection;
                return true;
            }
        }
        case Direction::Down:
        {
						// what you need to do when the current direction of the snake is Down
						// and the user inputs a new direction? TODO
            if(this->mDirection == Direction::Up || this->mDirection == Direction::Down) return false;
            else
            {
                this->mDirection = newDirection;
                return true;
            }
        }
        case Direction::Left:
        {
						// what you need to do when the current direction of the snake is Left
						// and the user inputs a new direction? TODO
            if(this->mDirection == Direction::Right || this->mDirection == Direction::Left) return false;
            else
            {
                this->mDirection = newDirection;
                return true;
            }
        }
        case Direction::Right:
        {
						// what you need to do when the current direction of the snake is Right
						// and the user inputs a new direction? TODO
            if(this->mDirection == Direction::Left || this->mDirection == Direction::Right) return false;
            else
            {
                this->mDirection = newDirection;
                return true;
            }
        }
    }

    return false;
}


SnakeBody Snake::createNewHead()
{
		/* TODO
		 * read the position of the current head
		 * read the current heading direction
		 * add the new head according to the direction
		 * return the new snake
		 */
    int newX,newY;
    switch(this->mDirection)
    {
        case Direction::Up:
            {
                newX = this->mSnake[0].getX();
                newY = this->mSnake[0].getY()-1;
                break;
            }
        case Direction::Down:
            {
                newX = this->mSnake[0].getX();
                newY = this->mSnake[0].getY()+1;
                break;
            }
        case Direction::Left:
            {
                newX = this->mSnake[0].getX()-1;
                newY = this->mSnake[0].getY();
                break;
            }
        case Direction::Right:
            {
                newX = this->mSnake[0].getX()+1;
                newY = this->mSnake[0].getY();
                break;
            }

    }
    this->mSnake.insert(this->mSnake.begin(),SnakeBody(newX,newY));



    SnakeBody newHead = this->mSnake[0];
    return newHead;
}

/*
 * If eat food, return true, otherwise return false
 */
bool Snake::moveFoward()
{
    /*
		 * TODO
		 * move the snake forward.
     * If eat food, return true, otherwise return false
     */
    if(this->touchFood() == false)
    {
        this->mSnake.pop_back();
        return false;
    }
    return true;





    //return false;
}

bool Snake::checkCollision()
{
    if (this->hitWall() || this->hitSelf())
    {
        return true;
    }
    else
    {
        return false;
    }
}


int Snake::getLength()
{
    return this->mSnake.size();
}

Direction Snake::getDirection()
{
    return this->mDirection;
}

bool Snake::isTooSmall()
{
    if(this->mSnake.size() < 1)
        return true;
    return false;
}
