
#include <time.h>
#include <math.h>
#include "Timer.h"

#include <iostream>
using namespace std;

void Timer::startTimer(){
    startTime = time(NULL);
}

void Timer::updateTime() {
    currentTime = time(NULL);
}

unsigned int Timer::getPlayTime() {
    tick = currentTime - startTime;
    return tick;
}

double Timer::getTick() {
    tick = currentTime - startTime;
    return tick;
}
