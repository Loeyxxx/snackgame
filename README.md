# 贪吃蛇

## 1. 项目描述
### 1.1 总述
本项目为在命令行中展示的贪吃蛇游戏，在普通贪吃蛇的基础上包括了游戏模式、毒药、障碍物、加分路径、生命值、地图等元素并添加音效。以下为游戏规则介绍。

### 1.2 游戏规则
#### 1.2.1 元素作用
+ **障碍物**：若直接碰撞，蛇生命值-1；若选择冲撞障碍物，在蛇并非为最短长度（2）的情况下，蛇穿过障碍物且自身长度-1。

+ **毒药**：

+ **加分路径**：

+ **生命值**：

+ **地图**：

+ **草**：



#### 1.2.2 操作指令
+ **转向**：上（W、w、↑），下（S、s、↓），左（A、a、←），右（D、d、→）
+ **冲撞障碍物**：C、c
+ **暂停**：Space/ Backspace
+ **继续**：Space/ Enter
+ **选项选择**：Space/ Enter


#### 1.2.3 模式规则
1. **无限模式**：生命值为1，矩形地图，随机生成4个障碍物，持续生成食物。有历史最高分排行榜（记录前三名）。

2. **闯关模式**

    未通关时从第一关开始，每当过关失败时回到第一关。已通关后可自主选择游玩关卡。


    + **第一关**：生命值为1，矩形地图，随机生成4个障碍物，持续生成食物。分数达到10即通过。
    + **第二关**：生命值为1，矩形地图，随机生成5个障碍物，持续生成食物、毒药。毒药数量为1。分数达到15即通过。
    + **第三关**：生命值为1，随机地图，随机生成4个障碍物，持续生成食物、毒药、加分路径、草。毒药数量为2。分数达到20即通过。
    + **第四关**：生命值为3，随机地图，随机生成4个障碍物，持续生成食物、毒药、加分路径。毒药数量为3。分数达到20即通过。
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

### 3.4 infiniteMode, levelOne ~ levelFive
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
首先调用backgroundMusic实现游戏背景音乐，将各关卡所需初始化条件实现。其中最后一行playTime为第五关通关检测条件。

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
？？？

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
判定是否碰撞地图边界或自身。如是，生命值-1，调用isDie判断生命值是否用尽，如未用尽则调用RestartGame复活，刷新计时器。其中改变playTime一行为时间通关条件特有，记录从第一条生命值开始游戏运行时间。

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
？？？

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
？？？
看你想不想把BloodPassage那一堆粘在下面了，不如直接说同上吧

```
```


```
        playTimer.updateTime();
        playBeforeTick = playTimer.getTick();

```
？？？

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

### 4.2 毒药
？？？不如你写同障碍物吧

### 4.3 加分路径

### 4.4 生命值

### 4.5 地图

### 4.6 草

### 4.7 音效
实现音效模块写于Sound.h，Sound.cpp。
```
#include <windows.h>
#include <Mmsystem.h>
#pragma comment(lib,"winmm.lib")
```
使用mciSendString和PlaySound函数所需的引用。

#### 4.7.1 void backgroundMusic(int mode)
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

#### 4.7.2 void soundEffect(int mode)
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
mode为1时播放吃食物音效，2为吃毒药，3为通过加分路径，4为获得新生命值，5为与墙、自身或障碍物发生碰撞，6为蛇死亡，7为通过某一关，8为通过全部关卡。9~10为蛇头在草地上运动，9为播放、10为终止。由于1~8音效可能在草的基础上播放，为避免播放冲突，草的音效使用mciSendString。


### 4.8 颜色


## 5. 其它
