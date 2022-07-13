# 贪吃蛇

## 1. 项目描述
### 1.1 总述
本项目为在命令行中展示的贪吃蛇游戏，在普通贪吃蛇的基础上包括了游戏模式、毒药、障碍物、加分路径、生命值、地图等元素并添加音效。以下为游戏规则介绍。

### 1.2 游戏规则
#### 1.2.1 元素作用
+ **障碍物**：若直接碰撞，蛇生命值-1；若选择冲撞障碍物，在蛇并非为最短长度（2）的情况下，蛇穿过障碍物且自身长度-1。记号为M。

+ **毒药**：蛇长度-1，分数-1。记号为X，为红色。

+ **加分路径**：增加分数，其增加值等于路径长度。记号为*，设置背景颜色为蓝。

+ **生命值**：沿规定路径走过后接触生命值才可时生命值+1。路径记号为O，生命值记号为&，设置背景颜色为紫。

+ **地图**：随机在已经设置好的地图中选择，设置背景颜色为白，白色区域蛇碰撞即死亡。

+ **草**：草地在游戏过程中随机生成/刷新，设置背景颜色为绿，进入草地蛇会加速，离开草地恢复原速，在草地上吃到食物蛇分数+2,长度仍然只+1。



#### 1.2.2 操作指令
+ **转向**：上（W、w、↑），下（S、s、↓），左（A、a、←），右（D、d、→）
+ **冲撞障碍物**：C、c
+ **暂停**：Space/ Backspace
+ **继续**：Space/ Enter
+ **选项选择**：Space/ Enter


#### 1.2.3 模式规则
1. **无限模式**：生命值为1，矩形地图，随机生成4个障碍物，矩形草地，持续生成食物，毒药，加分通道。有历史最高分排行榜（记录前三名）。

2. **闯关模式**

    未通关时从第一关开始，每当过关失败时回到第一关。已通关后可自主选择游玩关卡。


    + **第一关**：生命值为1，矩形地图，随机生成4个障碍物，持续生成食物。分数达到10即通过。
    + **第二关**：生命值为1，矩形地图，随机生成5个障碍物，持续生成食物、毒药、草。毒药数量为1。分数达到15即通过。
    + **第三关**：生命值为1，随机地图，随机生成4个障碍物，持续生成食物、毒药、加分路径、草。毒药数量为2。分数达到20即通过。
    + **第四关**：生命值为3，随机地图，随机生成4个障碍物，持续生成食物、毒药、加分路径。毒药数量为3。分数达到25即通过。
    + **第五关**：初始生命值为2，随机地图，随机生成5个障碍物，持续生成食物、毒药、新生命值。毒药数量为3。存活时间达60秒即通过。

## 2. 环境配置
本项目编写时使用mingw编译器，需要连接pdcurses库和libwinmm.a库。

配置方法（CodeBlocks）：

Settings->Compiler->Search Directories->Add，添加PDCurses.lib和libwinmm.a所在的路径。（后者一般可以在IDE安装目录下.\MinGW\x86_64-w64-mingw32\lib中找到）

Settings->Compiler->Linker Settings->Add，添加pdcurses.lib和libwinmm.a。


## 3. 运行步骤
以下为运行main、对game进行初始化后的函数按调用顺序进行介绍。部分功能的实现过程会在下一部分进行解释。

### 3.1 startGame
```
this->mainMenu();
this->runGame();
```
本游戏有无限和闯关两种模式，开始游戏时首先调用mainMenu函数进入模式选择界面，并通过私有成员(int) modeChoice标记所选模式，0为退出程序，1为无限，2为闯关。模式选择结束后调用runGame函数。

### 3.2 runGame
```
    if (this->modeChoice == 0) {}           // Choose "quit"

    else
    {
        while (this->modeChoice == 1)       // Choose "infinite"
        {
            this->readLeaderBoard();
            this->renderBoards();
            this->initializeGame();
            this->infiniteMode();
            this->updateLeaderBoard();
            this->writeLeaderBoard();
            this->renderRestartMenu();
        }

        while (this->modeChoice == 2)       // Choose "level mode"
        {
            wclear(this->mWindows[0]);
            wclear(this->mWindows[1]);
            wclear(this->mWindows[2]);
            this->levelMode();
        }
    }

```
若modeChoice为0，runGame不执行任何操作并结束，程序回到startGame终点并结束。

若modeChoice为1，在调用infiniteMode进行整个游戏过程前读取排行榜并打印各个告示板，之后将分数计入纪录并更新排行榜。游戏过程步骤展示于后文。

若modeChoice为2，清屏并调用levelMode。

### 3.3 levelMode
```
    if (this->hasWon == true)
    {
        this->renderChooseLevelMenu();
    }
    else
    {
        this->modeChoice = 5;   // start from level one
    }
```
(bool) hasWon记录是否通关。若已通关则调用renderChooseLevelMenu选关并将modeChoice更改至与关卡相对应的值（0：退出，5：第一关，6：第二关，以此类推）；若未通关则将modeChoice设为对应第一关的5。
```
    switch (this->modeChoice)
    {
        case 5:
        {
            if (levelOne())
            {
                this->renderNextLevelBoard();
            }
            else
            {
                this->modeChoice = 3;    // Fail
                break;
            }
        }
        ...
        case 9:
        {
            if (levelFive())
            {
                this->modeChoice = 4;   // Win
                break;
            }
            else
            {
                this->modeChoice = 3;    // Fail
                break;
            }
        }
    }
```
利用switch case实现关卡函数levelOne~levelFive从前到后的调用，若有关卡失败返回false则将modeChoice改为3，通过最后一关则将modeChoice改为4。
```
    switch (this->modeChoice)
    {
        case 3:     // Fail
        {
            this->renderRestartMenu();
            break;
        }
        case 4:     // Win
        {
            if (this->hasWon == false)
            {
                this->hasWon = true;
            }
            this->renderWinMenu();
        }
    }
```
若modeChoice==3则表示闯关失败，调用renderRestartMenu选择是否重新开始游戏。若为4则表示通关，调用renderWinMenu展示通关信息。levelMode结束。

### 3.4 initializeGame
```
		this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));

```
蛇指针重新生成。

```
    switch (this->modeChoice)
    {
        case 1:
        case 5:
        {
            std::vector<SnakeBody>().swap(this->mMap);
            this->createMapBoard();
            this->mPtrSnake->senseMap(this->mMap);
            break;
        }
        case 6:
        {
            std::vector<SnakeBody>().swap(this->mMap);
            this->createMapBoard();
            this->mPtrSnake->senseMap(this->mMap);
            this->createRandomGrass();
            break;
        }
        case 7:
        case 8:
        case 9:
        {
            std::vector<SnakeBody>().swap(this->mMap);
            this->createRandomMap();
            this->mPtrSnake->senseMap(this->mMap);
            this->createRandomGrass();
            break;
        }
    }
```
生成每个关卡对应的地图，无限模式与第一关只需生成矩形边框，第二关生成矩形边框和草地，三到五关生成随机地图和草地。生成后调用sense使蛇后续能进行对地形的判定。

```
    std::vector<int>().swap(this->mPoints);
    this->mPoints.push_back(0);
    this->mDifficulty = 0;
    this->mDelay = this->mBaseDelay;
    this->createRandomFood();
    this->mPtrSnake->senseFood(this->mFood);

    this->mSnakeLife = 1;
```
对分数、难度等级、刷新时间、食物和生命值进行初始化。

### 3.5 infiniteMode, levelOne ~ levelFive
由于各关难度递增且包含游戏元素递增，该六个函数大致有从后向前包含的特点，故在此摘取各关键片段进行整合解释。

```
    backgroundMusic(1);

    this->initializeGame();
    this->renderBoards();
    this->renderMap();

    //obstacle information
    this->mObstacleNum = 5;
    this->createRandomObstacle();
    this->mPtrSnake->senseObstacle(this->mObstacles);
    this->renderObstacle();

    //poison information
    this->PoisonLevel = 3;
    this->appearPoison();

    //time information
    Timer playTimer;
    time_t playBeforeTick, playNowTick;
    playTimer.startTimer();
    
    //Passage information
    std::vector<SnakeBody> PassageWalkThrough;
    Timer PassageTimer;
    time_t PassageTick;

    //Blood information
    this->mSnakeLife = 2;
    std::vector<SnakeBody> BloodWalkThrough;
    Timer BloodTimer;
    time_t BloodTick;
    this->renderLife();

    //Grass information
    int flag=0; //flag为0表示之前不在草上

    double playTime = 0;
```
首先调用backgroundMusic实现游戏背景音乐，将各关卡所需初始化条件实现。其中最后一行playTime为第五关勇于检测是否通关的条件。

```
    while (true)
    {
        ...
    }
    return false;
```
之后进入循环，每循环一次即为游戏状态刷新一次。最后一行返回false仅为闯关模式所有，代表未通关。...内容见下。

```
        playTimer.updateTime();
        playNowTick = playTimer.getTick();

        if(playNowTick != playBeforeTick && playNowTick % 4 == 0)   // faster poison
        {
            this->appearPoison();
        }

```
为该轮游戏过程生成新计时器、生成毒药。

```
        bool eatFood = this->mPtrSnake->moveFoward();
        if (eatFood)
        {
            soundEffect(1);
        }
        
        //检验是否在草地上
        if(playNowTick != playBeforeTick && playNowTick % 15 == 0 && std::rand() % 2 == 0)
        {
            this->appearGrass();
        }
        if(isPartOfGrass(this->mPtrSnake->getHead()) && flag==0)
        {
            flag=1 ;//flag为1表示之前在草地上
            this->GrassDelay();
        }

        if (flag == 1)
        {
            soundEffect(9);
        }
        else
        {
            soundEffect(10);
        }

        if(!isPartOfGrass(this->mPtrSnake->getHead()) && flag==1) //检验是否离开草地
        {
            flag=0;
            this->adjustDelay();
        }
        if(eatFood == true && flag==0)
        {
            int nowPoint = this->mPoints.back();
            this->mPoints.pop_back();
            nowPoint++;
            this->mPoints.push_back(nowPoint);
            this->createRandomFood();
            this->mPtrSnake->senseFood(this->mFood);
            this->adjustDelay();
        }
        if(eatFood == true && flag ==1)//在草地上吃食物获得额外加分
        {
            int nowPoint = this->mPoints.back();
            this->mPoints.pop_back();
            nowPoint+=2;
            this->mPoints.push_back(nowPoint);
            this->createRandomFood();
            this->mPtrSnake->senseFood(this->mFood);
            this->adjustDelay();
            this->GrassDelay();
        }

```
每15s刷新一次草地，flag表示蛇在草上（flag=1）和蛇不在草上（flag=1）的状态，蛇头在移动后首次进入草->触发音效+蛇加速，蛇头在移动后离开草地->蛇恢复速度。如果蛇在草上（flag=1）的时候吃到了食物->加2分+蛇长度加1。实现吃到食物、进入草地和离开草地时的音效控制。

```
        if (this->mPtrSnake->checkCollision())
        {
            soundEffect(5);
            this->mSnakeLife--;
            if(this->isDie())
                break;
            playTime += playTimer.getTick();
            this->RestartGame();
            playTimer.startTimer();
            continue;
        }

```
判定是否碰撞地图边界或自身。如是，生命值-1，调用isDie判断生命值是否用尽，如未用尽则调用RestartGame复活，刷新计时器。其中playTime += playTimer.getTick()为时间通关条件特有，记录从第一条生命值开始游戏运行时间。

```
        if (this->mPtrSnake->hitObstacle())
        {
            soundEffect(5);
            if (!this->crashObstacle())
            {
                this->mSnakeLife--;
                if(this->isDie())
                    break;
                this->RestartGame();
                playTimer.startTimer();
                continue;
            }
        }

```
判定是否碰撞障碍物。如是，调用crashObstacle判断是否成功冲撞障碍物。如未冲撞则结果同上述checkCollision后续。

```
        if(this->mPtrSnake->touchPoison()== true)
        {
            soundEffect(2);

            if(this->mPtrSnake->getLength() <= 2)   // Snake can't be shorter anymore
            {
                this->mSnakeLife--;
                if(this->isDie())
                    break;
                this->RestartGame();
                playTimer.startTimer();
                continue;
            }

            this->mPtrSnake->minusSnake();
            int nowPoint = this->mPoints.back();
            this->mPoints.pop_back();
            nowPoint--;
            this->mPoints.push_back(nowPoint);

            this->adjustDelay();
            this->appearPoison();
        }

```
判定是否碰到毒药。如是，首先判断蛇的长度是否大于2（因为碰到毒药蛇长度-1),若蛇的长度为2，减1之后直接死亡，重新开始该轮游戏；若蛇的长度大于2，蛇的长度-1，分数-1；

```
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
                    soundEffect(3);
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
```
若没有加分路径，则每7秒刷新且随机判断能否出现加分路径。当出现加速路径时开始计时，15s后该路径自动消失。如果蛇可以在路径存在的时间内，将路径完整的走完（中间不允许离开该路径），则获得额外的加分（加分=路径的长度）

```
        if(playNowTick != playBeforeTick && playNowTick % 10 == 0 && std::rand() % 2 == 0)
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
                //检查是否是从头开始
                SnakeBody Position = this->mPtrSnake->getHead();
                if (BloodWalkThrough.size() == 1 && !(Position == this->mBloodPassage.back()))
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
                    soundEffect(4);
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

```
若没有加血道具，则每10秒随机判断能否出现加血道具。当出现加血道具时开始计时，15s后该路径自动消失。如果蛇可以在道具存在的时间内，将道具前面的路完整的走完（中间不允许离开该路径，且必须从尾巴（即道具&）处离开），则生命值+1。此部分内容与加分路径对应部分几乎一致，仅有刷新时间与18行开始的从头进入路径判定有所区别，后者将在第四部分具体功能实现中加以解释。

```
        playTimer.updateTime();
        playBeforeTick = playTimer.getTick();

```
在每一轮的最后获得时间，在进行下一轮的时候这个相当于上一轮的时间，因为time_t是精确到小数点后的，但我们只精确到秒，所以要取int，且判断上一轮和下一轮属于不同的秒->当playBeforeTick!=playNowTick时才相当于真正的1s。

```
        if (this->mPoints.back() >= PassCondition)    // pass condition
        {
            backgroundMusic(0);
            return true;
        }
    }

    return false;
}

```
```
        if (playNowTick != playBeforeTick && playNowTick >= PassCondition)     // pass condition
        {
            backgroundMusic(0);
            return true;
        }

```
以上两个分别为分数和时间通关条件，判断为真则return true表示通过此关。

以上为游戏过程。

### 3.5 RestartGame
```
    this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));
    this->mPoints.push_back(0);
    this->mDifficulty = 0;
    this->createRandomFood();
    this->mPtrSnake->senseFood(this->mFood);
    this->mPtrSnake->senseObstacle(this->mObstacles);
    this->mPtrSnake->senseMap(this->mMap);
    this->mDelay = this->mBaseDelay;
    this->appearPoison();
    werase(this->mWindows[2]);
    this->renderBoards();
```
蛇指针重新生成，并对该生命值对应分数、难度等级重置，进行对各个元素的初始化。


## 4. 功能实现过程
以下选取部分函数进行解释。

### 4.0 判断地图中可生成新物体的位置
#### Game::isValidDot
调用isPartOfMap，isPartOfSnake一系列函数判断某一点是否是地图上已有元素的一部分，如每一个isPartOf结果均为false则该点可用。

#### Game::createAvailableGrids
生成包含所有可用点的向量(mAvailable)。
```
    std::vector<SnakeBody>().swap(this->mAvailable);
```
生成前首先清空。

```
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
                this->mAvailable.push_back(SnakeBody(j, i));
            }
        }
    }
```
遍历地图中每个点，若可用则加入mAvailable。

### 4.1 障碍物
#### Game::createRandomObstacle
```
    std::vector<SnakeBody>().swap(this->mObstacles);
```
生成包含障碍物的向量（mObstacles）前首先将其清空。

```
    for (int i = 0; i < this->mObstacleNum; i++)
    {
        this->createAvailableGrids();
        int random_index = std::rand() % this->mAvailable.size();
        this->mObstacles.push_back(this->mAvailable[random_index]);
    }
```
通过生成Available Grid、随机挑选一个点来生成障碍物。循环次数为障碍物数量。

#### Snake::senseObstacle 
```
    this->mObstacles = Obstacles;
```
使Snake类中代表障碍物的私有成员与Game中一致。
  
#### Game::crashObstacle
判断蛇是否成功冲撞障碍物。
```
    if (this->mPtrSnake->getLength() == 2) return false; 
```
蛇已处于最短长度，不能成功冲撞。

```
    int key = getch();
    if (key == 67 || key == 99)
    {
        this->mPtrSnake->minusSnake();
        return true;
    }
    return false;
```
若C键在碰撞时被按下，调用minusSnake使蛇长度-1并返回true成功冲撞。

### 4.2 计时功能
#### Timer
```
void Timer::startTimer(){
    startTime = time(NULL);
}
```
获得开始时间(系统时间，避免收到mDelay的影响)

**void Timer::updateTime()**
```
    currentTime = time(NULL);
```
更新时间，获得现在的时间。


```
unsigned int Timer::getPlayTime() 
{
    tick = currentTime - startTime;
    return tick;
}

double Timer::getTick() 
{
    tick = currentTime - startTime;
    return tick;
}
```
前者获得int->取整->秒数，后者获得double->精确到秒后的小数点。

### 4.3 毒药
生成过程与上述障碍物生成一致。

### 4.4 加分路径
#### Game::judgeDirection
获得lines最后一条线的方向。
```
    if(lines.size() <= 1)
        return (std::rand() % 4) +1;
```
size<=1，只有一个点，可以认为是随机方向。

```
    SnakeBody LastDot = lines.back();
    SnakeBody Last2ndDot = lines.at(lines.size()-2);
```
  为横线，且最后一个点的左边没有点，返回1
```
    if(LastDot.getY() == Last2ndDot.getY() && LastDot.getX() < Last2ndDot.getX())
        return 1;
```
  为横线，且最后一个点的右边没有点，返回2
```
    if(LastDot.getY() == Last2ndDot.getY() && LastDot.getX() > Last2ndDot.getX())
        return 2;
```
 为竖线，且最后一个点的上面没有点，返回3
```
    if(LastDot.getX() == Last2ndDot.getX() && LastDot.getY() < Last2ndDot.getY())
        return 3;
```
为竖线，且最后一个点的上面没有点，返回4
```
    if(LastDot.getX() == Last2ndDot.getX() && LastDot.getY() > Last2ndDot.getY())
        return 4;
```

####  Game::createNextLine
```
    SnakeBody Dot = lines.back();
    int Ndirection = judgeDirection(lines);
```
获得最后一个点并判断方向
```
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
```
根据方向，随机在已经判定出的方向上将直线进行延伸

其他方向类似，不再具体说明

####  Game::createTurningDot
```
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
```
判断方向后，如果为竖线，增加横向的点（使直线随机左拐/右拐）

横线类似，不再具体说明。

#### Game::createRandomPassage
```
   std::vector<SnakeBody>().swap(this->mPassage);
```
首先把之前的passage清空。

```
    std::vector<SnakeBody> availableGrid;
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
                availableGrid.push_back(SnakeBody(j, i));
            }
        }
    }
    int random_idx = std::rand() % availableGrid.size();
    this->mPassage.push_back(availableGrid[random_idx]);
```
获得初始点。

```
    for (int i=0;i < (std::rand() %4) +1 ; i++)
    {
        this->createNextLine(this->mPassage);
        this->createTurningDot(this->mPassage);
    }
```
一个循环是进行line的延伸+转折；随机进行x个循环

```
     if(this->mPassage.size() < 3 )
        std::vector<SnakeBody>().swap(this->mPassage);
```
如果长度太小，判定为这次生成失败，清空passage

#### Game::renderPassage
```
    init_color(COLOR_BLUE,3,168,158);
    wattron(this->mWindows[1],COLOR_PAIR(PASSAGE_PAIR));
    for(SnakeBody Passage : this->mPassage)
    {
        if(!this->mPtrSnake->isPartOfSnake(Passage.getX(),Passage.getY()))
            mvwaddch(this->mWindows[1],Passage.getY(), Passage.getX(), this->mPassageSymbol);
    }
    wattroff(this->mWindows[1],COLOR_PAIR(PASSAGE_PAIR));
    wrefresh(this->mWindows[1]);
```
如果在passage上有蛇，则print蛇，屏幕上表现为蛇会覆盖掉身下的passage

#### 对于走完passage的判定
```
if(this->mPassage.size()>0)
```
存在passage才会进入后续判定语句

```
PassageWalkThrough = this->mPassage;
```
PassageWalkThrough初始化为mPassage，如果蛇走过mPassage其中一格，则PassageWalkThrough去掉这一格，作为记录。

```
      if(!isPartOfPassage(this->mPtrSnake->getHead()) && PassageWalkThrough.size() != this->mPassage.size())
        {
            PassageWalkThrough = this->mPassage;
        }
```
如果蛇不在Passage上但PassageWalkThrough小于passage->这里可以认为是之前蛇已经走了passage的部分但却在此时离开了passage，所以重置PassageWalkThrough。

```
 if(isPartOfPassage(this->mPtrSnake->getHead()))
```
蛇在passage上则进入后续操作

- 后续操作
```
       SnakeBody Position = this->mPtrSnake->getHead();

       if(!is_inVector(PassageWalkThrough, Position))
              {
                PassageWalkThrough = this->mPassage;
              }
```
如果所处的position并不在PassageWalkThrough—>表示这个地方被蛇重复走过，不符合要求，重置PassageWalkThrough。

```
            else
                {
                    std::vector<SnakeBody>::iterator pos;
                    pos = find(PassageWalkThrough.begin(), PassageWalkThrough.end(), Position);
                    if(pos != PassageWalkThrough.end())
                        PassageWalkThrough.erase(pos);
                    else PassageWalkThrough.pop_back();
                }
```
走过的position在PassageWalkThrough中删除，留作记录。

```
      if(PassageWalkThrough.size() == 0)
                {
                    soundEffect(3);
                    int nowPoint = this->mPoints.back();
                    this->mPoints.pop_back();
                    nowPoint += (int)(this->mPassage.size());
                    this->mPoints.push_back(nowPoint);
                    std::vector<SnakeBody>().swap(this->mPassage);
                    std::vector<SnakeBody>().swap(PassageWalkThrough);
                    this->adjustDelay();
                }
```
当走完通道后->触发音效+额外加分。

#### Game::is_inVector
```
    for(SnakeBody position : walkThrough)
    {
        if(position == Position)
            return true;
    }
    return false;
```
判断是否在walkThrough里。

### 4.5 生命值
#### Game::RestartGame
```
    this->mPtrSnake = nullptr;
    this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight,     this->mInitialSnakeLength));
    this->mPoints.push_back(0);
    this->mDifficulty = 0;
    this->createRandomFood();
    this->mPtrSnake->senseFood(this->mFood);
    this->mPtrSnake->senseObstacle(this->mObstacles);
    this->mPtrSnake->senseMap(this->mMap);
    this->mDelay = this->mBaseDelay;
    this->appearPoison();
    werase(this->mWindows[2]);
    this->renderBoards();
```
当蛇的长度<2时，判定为这一轮蛇死亡，生命值-1并初始化该轮游戏。

#### Game::isDie
```
    if(this->mSnakeLife == 0)
    {
        backgroundMusic(0);
        soundEffect(6);
        return true;
    }
    return false;
```
当生命值为0的时候触发音效，结束游戏。

---
- 加血道具

获得加血道具必须走完完整的通道，实现过程与passage几乎一致，

**但必须从头进入**

在此只说明如何实现从头进入的判定。
```
    if (BloodWalkThrough.size() == 1 && !(Position == this->mBloodPassage.back()))
                {
                    BloodWalkThrough = this->mBloodPassage;
                }
```
当BloodWalkThrough长度为1时，表示蛇已经走到通道的尾端，为实现从尾端离开，BloodWalkThrough剩余的必须和mBloodPassage的最后一个相等，如果不相等，则没有从头进入->重置BloodWalkThrough

### 4.6 地图
#### Game::createMapBoard
```
for(int i=1;i<this->mGameBoardWidth;i++)
    {
        this->mMap.push_back(SnakeBody(i,1));
    }
    for(int i=1;i<this->mGameBoardWidth;i++)
    {
        this->mMap.push_back(SnakeBody(i,this->mGameBoardHeight-1));
    }
    for(int i=1;i<this->mGameBoardWidth;i++)
    {
        this->mMap.push_back(SnakeBody(1,i));
    }
    for(int i=1;i<this->mGameBoardWidth;i++)
    {
        this->mMap.push_back(SnakeBody(this->mGameBoardWidth-1,i));
    }
```
创建地图边界。

#### Game::createRandomMap
```
    this->createMapBoard();
    std::vector<std::vector<SnakeBody> > AllMap;
```
首先都要进行边界的设置，AllMap储存已经提前设定好的地图。
```
    AllMap.push_back(Map3);
    int random_idx = std::rand() % AllMap.size();
    this->mMap.insert(this->mMap.end(),AllMap[random_idx].begin(),AllMap[random_idx].end());
```
随机选择地图。

### 4.7 草
#### Game::isValidGrass
```
   int DotX=Dot.getX(),DotY=Dot.getY();

    if(isPartOfMap(Dot))
    {
        return false;
    }

    if (DotX < 1 || DotX >= this->mGameBoardWidth - 1 || DotY < 1 || DotY >= this->mGameBoardHeight -1)
    {
        return false;
    }
    return true;
```
判定能否生成草，草和地图属于同种类型，因此只要满足不在地图上不超过地图即可
#### Game::createRandomGrass
```
    std::vector<SnakeBody>().swap(this->mGrass);
    int random_width = std::rand()%20+5;
    int random_height= std::rand()%10+5;
    int random_widthstart = std::rand()%this->mGameBoardWidth;
    int random_heightstart = std::rand()%this->mGameBoardHeight;
    for(int i=0;i<random_width;i++)
    {
        for(int j=0;j<random_height;j++)
        {
            if(isValidGrass(SnakeBody(i+random_widthstart,j+random_heightstart)) )
                this->mGrass.push_back(SnakeBody(i+random_widthstart,j+random_heightstart));
        }
    }
```
创建随机矩形草地。

#### Game::GrassDelay
```
this->mDelay *= 0.5;
```
进入草地加速。

### 4.8 音效
实现音效模块写于Sound.h，Sound.cpp。
```
#include <windows.h>
#include <Mmsystem.h>
#pragma comment(lib,"winmm.lib")
```
使用mciSendString和PlaySound函数所需的引用。

#### 4.8.1 void backgroundMusic(int mode)
mode为控制背景音乐播放的选项。
```
    switch (mode)
    {
        case 0:
        {
            mciSendString("stop .\\Sound\\Elsinore-Revisited.mp3", NULL, 0, NULL);
            break;
        }
        case 1:
        {
            mciSendString("play .\\Sound\\Elsinore-Revisited.mp3 repeat", NULL, 0, NULL);
            break;
        }
        case 2:
        {
            mciSendString("pause .\\Sound\\Elsinore-Revisited.mp3", NULL, 0, NULL);
            break;
        }
```

0~2为游戏背景音乐控制，其中0为终止、1为播放、2为暂停。

```
        case 3:
        {
            mciSendString("stop .\\Sound\\snake.mp3", NULL, 0, NULL);
            break;
        }
        case 4:
        {
            mciSendString("play .\\Sound\\snake.mp3 repeat", NULL, 0, NULL);
            break;
        }
    }
```
3~4为主菜单背景音乐控制，其中3为终止、4为播放。

#### 4.8.2 void soundEffect(int mode)
控制播放与游戏元素有关的音效。
```
    switch (mode)
    {
        case 1:
        {
            PlaySound(TEXT(".\\Sound\\eat.wav"),NULL,SND_FILENAME | SND_ASYNC);
            break;
        }
        ...
        case 9:
        {
            mciSendString("play .\\Sound\\grass.mp3 repeat", NULL, 0, NULL);
            break;
        }
        case 10:
        {
            mciSendString("stop .\\Sound\\grass.mp3", NULL, 0, NULL);
            break;
        }
    }
```

mode为1时播放吃食物音效，2为吃毒药，3为通过加分路径，4为获得新生命值，5为与墙、自身或障碍物发生碰撞，6为蛇死亡，7为通过某一关，8为通过全部关卡。9、10为蛇头在草地上运动，9为播放、10为终止。由于1~8音效可能在草的基础上播放，为避免播放冲突，草的音效使用mciSendString。


### 4.9 颜色
#### Game::renderColorMenu()
前面和其他Menu一样，在此不再赘述
```
std::vector<std::string> menuItems = {" Yellow", " Brown"," Blue", " Quit"};
```
提供三种颜色和退出选项
```
if (index == 3)//quit
    {
        exit(0);
    }
    if (index == 2)
    {
        init_color(COLOR_YELLOW,175,238,238);
    }
    if( index==1)
    {
        init_color(COLOR_YELLOW,255,165,0);
    }
    if(index==0)
    {

    }
```
根据选择对COLOR_YELLOW进行RGB配色

*整个游戏中只有蛇的颜色用到了COLOR_YELLOW，所以不会对其他物品造成影响*

### 4.10 分数
```
std::vector<int> mPoints;
```
通过vector来记录每一条生命值所对应的分数。

**每一轮只要调用mPoints.back()就可以获得该生命值的分数**

**每一条生命值初始化的时候都要this->mPoints.push_back(0)**

#### Game::AccumulatePoints
```
    int ans=0;
    for(int point: this->mPoints)
    {
        ans += point;
    }
    return ans;
```
实现计算总分。

## 5. 其它
### 5.1 内存管理
