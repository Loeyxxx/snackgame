#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

typedef long clock_t;
class Timer {
public:
    clock_t startTime;
    clock_t currentTime;
    time_t tick;

    Timer() {};

    ~Timer() {};

    void startTimer();

    void updateTime();

    unsigned int getPlayTime();

    double getTick();

};

#endif // TIMER_H_INCLUDED
