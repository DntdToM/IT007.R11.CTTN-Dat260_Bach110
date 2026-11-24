#include <stdio.h>
#include <stdlib.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[]);
void printProcess(int n, PCB P[]);
void exportGanttChart(int n, PCB P[]);
void pushProcess(int *n, PCB P[], PCB Q);
void removeProcess(int *n, int index, PCB P[]);
int swapProcess(PCB *P, PCB *Q);
int partition(PCB P[], int low, int high, int iCriteria);
void quickSort(PCB P[], int low, int high, int iCriteria);
void calculateAWT(int n, PCB P[]);
void calculateATaT(int n, PCB P[]);

void inputProcess(int n, PCB P[]) {
    for (int i = 0; i < n; i++) {
        printf("Process P%d:\n", i + 1);
        P[i].iPID = i + 1;
        printf("  Arrival Time: ");
        scanf("%d", &P[i].iArrival);
        printf("  Burst Time: ");
        scanf("%d", &P[i].iBurst);
        P[i].iStart = 0;
        P[i].iFinish = 0;
        P[i].iWaiting = 0;
        P[i].iResponse = 0;
        P[i].iTaT = 0;
    }
}

void printProcess(int n, PCB P[]) {
    printf("\nPID\tArrival\tBurst\tStart\tFinish\tWaiting\tResponse\tTaT\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t\t%d\n",
               P[i].iPID, P[i].iArrival, P[i].iBurst,
               P[i].iStart, P[i].iFinish, P[i].iWaiting,
               P[i].iResponse, P[i].iTaT);
    }
}

void exportGanttChart(int n, PCB P[]) {
    if (n <= 0) return;
    printf("\n--- Gantt Chart ---\n");

    if (P[0].iStart > 0) printf("-------");
    for (int i = 0; i < n; i++) {
        if (i > 0 && P[i].iStart > P[i - 1].iFinish) {
            printf("-------");
        }
        printf("-------");
    }
    printf("-\n|");

    if (P[0].iStart > 0) printf(" IDLE |");
    for (int i = 0; i < n; i++) {
        if (i > 0 && P[i].iStart > P[i - 1].iFinish) {
            printf(" IDLE |");
        }
        printf("  P%-2d |", P[i].iPID);
    }
    printf("\n");

    if (P[0].iStart > 0) printf("-------");
    for (int i = 0; i < n; i++) {
        if (i > 0 && P[i].iStart > P[i - 1].iFinish) {
            printf("-------");
        }
        printf("-------");
    }
    printf("-\n");

    if (P[0].iStart > 0) {
        printf("%-7d%-7d", 0, P[0].iStart);
    } else {
        printf("%-7d", P[0].iStart);
    }
    for (int i = 0; i < n; i++) {
        if (i > 0 && P[i].iStart > P[i - 1].iFinish) {
            printf("%-7d", P[i].iStart);
        }
        printf("%-7d", P[i].iFinish);
    }
    printf("\n");
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
    return 0;
}

int partition(PCB P[], int low, int high, int iCriteria) {
    PCB pivot = P[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        int condition = 0;
        switch (iCriteria) {
        case SORT_BY_ARRIVAL:
            if (P[j].iArrival < pivot.iArrival ||
               (P[j].iArrival == pivot.iArrival && P[j].iPID < pivot.iPID))
                condition = 1;
            break;
        case SORT_BY_PID:
            if (P[j].iPID < pivot.iPID)
                condition = 1;
            break;
        case SORT_BY_BURST:
            if (P[j].iBurst < pivot.iBurst ||
               (P[j].iBurst == pivot.iBurst && P[j].iArrival < pivot.iArrival))
                condition = 1;
            break;
        case SORT_BY_START:
            if (P[j].iStart < pivot.iStart ||
               (P[j].iStart == pivot.iStart && P[j].iPID < pivot.iPID))
                condition = 1;
            break;
        }

        if (condition) {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return i + 1;
}

void quickSort(PCB P[], int low, int high, int iCriteria) {
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void calculateAWT(int n, PCB P[]) {
    int totalWaiting = 0;
    for (int i = 0; i < n; i++) {
        totalWaiting += P[i].iWaiting;
    }
    printf("Average Waiting Time: %.2f\n", (float)totalWaiting / n);
}

void calculateATaT(int n, PCB P[]) {
    int totalTaT = 0;
    for (int i = 0; i < n; i++) {
        totalTaT += P[i].iTaT;
    }
    printf("Average Turnaround Time: %.2f\n", (float)totalTaT / n);
}

int main() {
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];
    int iNumberOfProcess;

    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;

    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);

    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    printf("\nReady Queue: ");
    printProcess(1, ReadyQueue);

    while (iTerminated < iNumberOfProcess) {
        while (iRemain > 0) {
            if (Input[0].iArrival <= ReadyQueue[0].iFinish) {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
                continue;
            } else
                break;
        }

        if (iReady == 0 && iRemain > 0) {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
            ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
            ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
            ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
            ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
            ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
        }

        if (iReady > 0) {
            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);

            if (iReady > 0) {
                ReadyQueue[0].iStart = TerminatedArray[iTerminated - 1].iFinish;
                ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
                ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
                ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
                ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
            }
        }
    }

    printf("\n===== FCFS Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printProcess(iTerminated, TerminatedArray);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}
