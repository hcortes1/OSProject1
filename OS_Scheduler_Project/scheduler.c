#include <stdio.h>
#include <stdlib.h>

#define MAX 100

typedef struct {
    int pid;
    int arrival;
    int burst;
    int priority;
    int waiting;
    int turnaround;
    int completed;
} Process;

Process processes[MAX];
int n;

void readProcesses() {
    FILE* file = fopen("processes.txt", "r");
    if (!file) {
        printf("Error: Could not open processes.txt\n");
        exit(1);
    }

    char header[100];
    fgets(header, sizeof(header), file); // skip header line

    n = 0;
    while (fscanf(file, "%d %d %d %d",
        &processes[n].pid,
        &processes[n].arrival,
        &processes[n].burst,
        &processes[n].priority) == 4) {
        processes[n].completed = 0;
        n++;
    }
    fclose(file);
}

void printResults() {
    printf("\n%-10s %-10s %-10s %-10s %-15s %-15s\n",
        "PID", "Arrival", "Burst", "Priority", "Waiting Time", "Turnaround Time");

    float totalWT = 0, totalTAT = 0;
    for (int i = 0; i < n; i++) {
        printf("%-10d %-10d %-10d %-10d %-15d %-15d\n",
            processes[i].pid,
            processes[i].arrival,
            processes[i].burst,
            processes[i].priority,
            processes[i].waiting,
            processes[i].turnaround);
        totalWT += processes[i].waiting;
        totalTAT += processes[i].turnaround;
    }

    printf("\nAverage Waiting Time: %.2f\n", totalWT / n);
    printf("Average Turnaround Time: %.2f\n", totalTAT / n);
}

void fcfs() {
    printf("\n==================== FCFS Scheduling ====================\n");

    // Sort by arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival > processes[j + 1].arrival) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    int time = 0;
    printf("\nGantt Chart:\n");
    printf("|");

    for (int i = 0; i < n; i++) {
        if (time < processes[i].arrival)
            time = processes[i].arrival;

        processes[i].waiting = time - processes[i].arrival;
        time += processes[i].burst;
        processes[i].turnaround = time - processes[i].arrival;
        printf(" P%d |", processes[i].pid);
    }

    printf("\n0");
    time = 0;
    for (int i = 0; i < n; i++) {
        if (time < processes[i].arrival)
            time = processes[i].arrival;
        time += processes[i].burst;
        printf("   %d", time);
    }
    printf("\n");

    printResults();
}

void sjf() {
    printf("\n==================== Shortest Job First (SJF) ====================\n");

    int completed = 0, time = 0;

    printf("\nGantt Chart:\n|");
    while (completed < n) {
        int idx = -1;
        int minBurst = 9999;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && !processes[i].completed) {
                if (processes[i].burst < minBurst) {
                    minBurst = processes[i].burst;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            time++;
            continue;
        }

        processes[idx].waiting = time - processes[idx].arrival;
        time += processes[idx].burst;
        processes[idx].turnaround = time - processes[idx].arrival;
        processes[idx].completed = 1;
        completed++;

        printf(" P%d |", processes[idx].pid);
    }

    printf("\n0");
    time = 0;
    for (int i = 0; i < n; i++) time += processes[i].burst;
    printf("   %d\n", time);

    printResults();
}

int main() {
    readProcesses();

    fcfs();

    // Reset process data for next algorithm
    for (int i = 0; i < n; i++) {
        processes[i].waiting = 0;
        processes[i].turnaround = 0;
        processes[i].completed = 0;
    }

    sjf();

    return 0;
}
