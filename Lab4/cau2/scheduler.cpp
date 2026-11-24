#include "scheduler.hpp"
#include <random>
#include <iomanip>

using namespace std;

// ================= PROCESS IMPLEMENTATION =================

process::process(int initBurst, int initArrival, int initPid) {
    burstTime = initBurst;
    arrivalTime = initArrival;
    pid = initPid;
    remainTime = initBurst;
    startTime = -1;
    finishTime = -1;
}

int process::getArrivalTime() { return arrivalTime; }
int process::getRemainTime()  { return remainTime; }
int process::getBurstTime()   { return burstTime; }
int process::getPid()         { return pid; }
int process::getStartTime()   { return startTime; }
int process::getFinishTime()  { return finishTime; }

int process::getResponse() {
    return startTime - arrivalTime;
}

int process::getTaT() {
    return finishTime - arrivalTime;
}

int process::getWaitTime() {
    return getTaT() - burstTime;
}

void process::setStartTime(int time) {
    startTime = time;
}

void process::setFinishTime(int time) {
    finishTime = time;
}

void process::executeProcess(int runningTime) {
    this->remainTime -= runningTime;
    cout << "Process P" << pid << " execute in CPU for " << runningTime << "s\n";
}

// ================= SCHEDULER IMPLEMENTATION =================

scheduler::scheduler(int processCount, int quantum, uint32_t seed) {
    timer = 0;
    quantumTime = quantum;

    mt19937 rng(seed);
    uniform_int_distribution<int> distArrival(0, 20); // Arrival Time trong [0, 20]
    uniform_int_distribution<int> distBurst(2, 12);   // Burst Time trong [2, 12]

    cout << "INITIAL PROCESSES (Quantum = " << quantumTime << ")\n";
    cout << "PID\t| ARRIVAL\t| BURST\n";
    cout << "-------------------------------\n";

    for (int i = 0; i < processCount; i++) {
        int randomArrival = distArrival(rng);
        int randomBurst = distBurst(rng);
        int pid = i + 1;

        process* newProcess = new process(randomBurst, randomArrival, pid);
        cout << "P" << pid << "\t| " << randomArrival << "\t\t| " << randomBurst << "\n";

        arrivalQueue.push(newProcess);
    }
    cout << "\n";
}

scheduler::~scheduler() {
    for (process* p : finishProcess) {
        delete p;
    }
    while (!waitQueue.empty()) {
        delete waitQueue.front();
        waitQueue.pop();
    }
    while (!arrivalQueue.empty()) {
        delete arrivalQueue.top();
        arrivalQueue.pop();
    }
}

void scheduler::runRR() {
    process* currentRunningProcess = nullptr;
    ganttLog.clear();

    // Nạp các tiến trình đến ngay tại thời điểm 0
    while (!arrivalQueue.empty() && timer >= arrivalQueue.top()->getArrivalTime()) {
        waitQueue.push(arrivalQueue.top());
        arrivalQueue.pop();
    }

    while (!arrivalQueue.empty() || !waitQueue.empty()) {
        // Xử lý khoảng trống CPU Idle nếu waitQueue rỗng
        if (waitQueue.empty()) {
            int nextArrival = arrivalQueue.top()->getArrivalTime();
            ganttLog.push_back({-1, timer, nextArrival}); // -1 đánh dấu IDLE
            timer = nextArrival;

            while (!arrivalQueue.empty() && timer >= arrivalQueue.top()->getArrivalTime()) {
                waitQueue.push(arrivalQueue.top());
                arrivalQueue.pop();
            }
        }

        // Lấy tiến trình đầu hàng đợi
        if (currentRunningProcess == nullptr) {
            currentRunningProcess = waitQueue.front();
            waitQueue.pop();

            if (currentRunningProcess->getBurstTime() == currentRunningProcess->getRemainTime()) {
                currentRunningProcess->setStartTime(timer);
            }
        }

        int runningTime = min(currentRunningProcess->getRemainTime(), quantumTime);
        int sliceStart = timer;
        timer += runningTime;
        currentRunningProcess->executeProcess(runningTime);

        // Ghi log Gantt (gộp nếu trùng PID liên tiếp)
        if (!ganttLog.empty() && ganttLog.back().pid == currentRunningProcess->getPid()) {
            ganttLog.back().end = timer;
        } else {
            ganttLog.push_back({currentRunningProcess->getPid(), sliceStart, timer});
        }

        // Nạp TẤT CẢ tiến trình mới đến trong lúc tiến trình hiện tại vừa chạy
        while (!arrivalQueue.empty() && timer >= arrivalQueue.top()->getArrivalTime()) {
            waitQueue.push(arrivalQueue.top());
            arrivalQueue.pop();
        }

        // Kiểm tra hoàn thành hoặc đẩy về cuối hàng đợi
        if (currentRunningProcess->getRemainTime() == 0) {
            currentRunningProcess->setFinishTime(timer);
            finishProcess.push_back(currentRunningProcess);
        } else {
            waitQueue.push(currentRunningProcess);
        }

        currentRunningProcess = nullptr;
    }
}

void scheduler::printGanttChart() {
    cout << "\n===== GANTT CHART =====\n|";
    for (const auto& block : ganttLog) {
        if (block.pid == -1) cout << " IDLE |";
        else cout << "  P" << block.pid << "  |";
    }
    cout << "\n0";
    for (const auto& block : ganttLog) {
        cout << setw(7) << block.end;
    }
    cout << "\n";
}

void scheduler::printResultTable() {
    sort(finishProcess.begin(), finishProcess.end(), [](process* a, process* b) {
        return a->getPid() < b->getPid();
    });

    cout << "\n===== PROCESS TABLE =====\n";
    cout << "PID\tArrival\tBurst\tStart\tFinish\tResponse\tTaT\tWaiting\n";
    for (process* p : finishProcess) {
        cout << "P" << p->getPid() << "\t"
             << p->getArrivalTime() << "\t"
             << p->getBurstTime() << "\t"
             << p->getStartTime() << "\t"
             << p->getFinishTime() << "\t"
             << p->getResponse() << "\t\t"
             << p->getTaT() << "\t"
             << p->getWaitTime() << "\n";
    }
    cout << fixed << setprecision(2);
    cout << "\nAverage Response Time   : " << averageResponse() << "\n";
    cout << "Average Turnaround Time : " << averageTaT() << "\n";
    cout << "Average Waiting Time    : " << averageWaitTime() << "\n";
}

float scheduler::averageWaitTime() {
    if (finishProcess.empty()) return 0;
    float sum = 0;
    for (process* p : finishProcess) sum += p->getWaitTime();
    return sum / finishProcess.size();
}

float scheduler::averageTaT() {
    if (finishProcess.empty()) return 0;
    float sum = 0;
    for (process* p : finishProcess) sum += p->getTaT();
    return sum / finishProcess.size();
}

float scheduler::averageResponse() {
    if (finishProcess.empty()) return 0;
    float sum = 0;
    for (process* p : finishProcess) sum += p->getResponse();
    return sum / finishProcess.size();
}