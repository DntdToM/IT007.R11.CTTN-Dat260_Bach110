#pragma once

#include <iostream>
#include <queue>
#include <algorithm>
#include <vector>
#include <cstdint>

class process {
private:
    int burstTime;
    int arrivalTime;
    int pid;
    int startTime;
    int remainTime;
    int finishTime;

public:
    int getArrivalTime();
    int getRemainTime();
    int getResponse();
    int getTaT();
    int getWaitTime();
    int getPid();
    int getBurstTime();
    int getStartTime();
    int getFinishTime();

    void setStartTime(int time);
    void setFinishTime(int time);
    void executeProcess(int runningTime = 0);

    process(int initBurst = 0, int initArrival = 0, int initPid = 0);
};

struct ArrivalCompare {
    bool operator()(process* a, process* b) const {
        if (a->getArrivalTime() == b->getArrivalTime()) {
            return a->getPid() > b->getPid();
        }
        return a->getArrivalTime() > b->getArrivalTime();
    }
};

struct GanttBlock {
    int pid;
    int start;
    int end;
};

class scheduler {
private:
    std::queue<process*> waitQueue;
    std::priority_queue<process*, std::vector<process*>, ArrivalCompare> arrivalQueue;
    std::vector<process*> finishProcess;
    std::vector<GanttBlock> ganttLog;
    int timer;
    int quantumTime;

public:
    void runRR();
    void printGanttChart();
    void printResultTable();
    float averageWaitTime();
    float averageTaT();
    float averageResponse();

    scheduler(int processCount = 5, int quantum = 5, uint32_t seed = 0);
    ~scheduler();
};