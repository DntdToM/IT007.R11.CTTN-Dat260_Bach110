#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        P[i].iPID = i + 1;
        P[i].iArrival = rand() % 21; // tạo arrivalTime ngẫu nhiên từ 0 -> 20
        P[i].iBurst = (rand() % 11) + 2; // tạo burstTime ngẫu nhiên từ 2 -> 12  
        P[i].iStart = 0;
        P[i].iFinish = 0;
        P[i].iWaiting = 0;
        P[i].iResponse = 0;
        P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[], int mode) {
    if (mode == 1){
        printf("\n+-----+--------------+------------+\n");
        printf("| PID | Arrival Time | Burst Time |\n");
        printf("+-----+--------------+------------+\n");
        for (int i = 0; i < n; i++) {
            printf("| P%-2d | %-12d | %-10d |\n",
                P[i].iPID, P[i].iArrival, P[i].iBurst);
        }
        printf("+-----+--------------+------------+\n");
        return;
    }
    printf("\n+-----+--------------+------------+------------+-------------+--------------+---------------+-----------------+\n");
    printf("| PID | Arrival Time | Burst Time | Start Time | Finish Time | Waiting Time | Response Time | Turnaround Time |\n");
    printf("+-----+--------------+------------+------------+-------------+--------------+---------------+-----------------+\n");
    for (int i = 0; i < n; i++) {
        printf("| P%-2d | %-12d | %-10d | %-10d | %-11d | %-12d | %-13d | %-15d |\n",
               P[i].iPID, P[i].iArrival, P[i].iBurst,
               P[i].iStart, P[i].iFinish, P[i].iWaiting,
               P[i].iResponse, P[i].iTaT);
    }
    printf("+-----+--------------+------------+------------+-------------+--------------+---------------+-----------------+\n");
}

void exportGanttChart(int n, PCB P[]) {
    printf("\nGantt Chart:\n");
    for (int i = 0; i < n; i++) {
        if (i == 0 && P[i].iStart > 0) {
            printf("| Idle ");
        } else if (i > 0 && P[i].iStart > P[i - 1].iFinish) {
            printf("| Idle ");
        }
        printf("|  P%d  ", P[i].iPID);
    }
    printf("|\n");

    for (int i = 0; i < n; i++) {
        if (i == 0 && P[i].iStart > 0) {
            printf("%-7d", 0);
        } else if (i > 0 && P[i].iStart > P[i - 1].iFinish) {
            printf("%-7d", P[i - 1].iFinish);
        }
        printf("%-7d", P[i].iStart);
    }
    if (n > 0) {
        printf("%d\n", P[n - 1].iFinish);
    }
}

void pushProcess(int *n, PCB P[], PCB Q) {
    P[*n] = Q;
    (*n)++;
}

void removeProcess(int *n, int index, PCB P[]) {
    for (int i = index; i < (*n) - 1; i++) {
        P[i] = P[i + 1];
    }
    (*n)--;
}

int swapProcess(PCB *P, PCB *Q) {
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}

int compareProcess(PCB a, PCB b, int iCriteria) {
    if (iCriteria == SORT_BY_ARRIVAL) {
        if (a.iArrival != b.iArrival) return a.iArrival < b.iArrival;
        if (a.iBurst != b.iBurst) return a.iBurst < b.iBurst;
        return a.iPID < b.iPID;
    } else if (iCriteria == SORT_BY_PID) {
        return a.iPID < b.iPID;
    } else if (iCriteria == SORT_BY_BURST) {
        if (a.iBurst != b.iBurst) return a.iBurst < b.iBurst;
        if (a.iArrival != b.iArrival) return a.iArrival < b.iArrival;
        return a.iPID < b.iPID;
    } else if (iCriteria == SORT_BY_START) {
        if (a.iStart != b.iStart) return a.iStart < b.iStart;
        return a.iPID < b.iPID;
    }
    return 0;
}

int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;
    for (int j = low; j <= high - 1; j++) {
        if (compareProcess(P[j], pivot, iCriteria)) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return (i + 1);
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void calculateAWT(int n, PCB P[]) {
    float sumWaiting = 0;
    for (int i = 0; i < n; i++) {
        sumWaiting += P[i].iWaiting;
    }
    printf("\nAverage Waiting Time (AWT): %.2f\n", sumWaiting / n);
}

void calculateATaT(int n, PCB P[]) {
    float sumTaT = 0;
    for (int i = 0; i < n; i++) {
        sumTaT += P[i].iTaT;
    }
    printf("Average Turnaround Time (ATaT): %.2f\n", sumTaT / n);
}

int main() {
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];
    int iNumberOfProcess;

    srand((unsigned int)time(NULL));

    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;

    inputProcess(iNumberOfProcess, Input);
    printf("\n===== INITIAL INPUT PROCESSES =====");
    printProcess(iNumberOfProcess, Input, 1);

    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);

    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    while (iTerminated < iNumberOfProcess) {
        while (iRemain > 0) {
            if (Input[0].iArrival <= ReadyQueue[0].iFinish) {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
                continue;
            } else {
                break;
            }
        }

        if (iReady > 0) {
            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);

            if (iReady == 0 && iRemain > 0) {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
            }

            if (iReady > 0) {
                quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);

                if (TerminatedArray[iTerminated - 1].iFinish < ReadyQueue[0].iArrival) {
                    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
                } else {
                    ReadyQueue[0].iStart = TerminatedArray[iTerminated - 1].iFinish;
                }
                ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
                ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
                ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
                ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
            }
        }
    }

    printf("\n===== SJF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);

    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printProcess(iTerminated, TerminatedArray, 0);

    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}