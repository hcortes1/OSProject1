// scheduler.c
// Compile: gcc -std=c99 -O2 -Wall -o scheduler scheduler.c
// Run: ./scheduler processes.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256

typedef struct {
    int pid;
    int arrival;
    int burst;
    int priority;
    int completion;
    int start_time;
} Process;

typedef struct {
    char label[10];
    int start;
    int end;
} Gantt;

// Function to read processes from file
Process* read_processes(const char *filename, int *count) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Error opening file");
        exit(1);
    }

    Process *list = NULL;
    int n = 0, cap = 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || isspace(line[0])) continue; // ignore comments
        Process p;
        if (sscanf(line, "%d %d %d %d", &p.pid, &p.arrival, &p.burst, &p.priority) == 4) {
            if (n >= cap) {
                cap = cap ? cap * 2 : 8;
                list = realloc(list, cap * sizeof(Process));
            }
            p.completion = 0;
            p.start_time = 0;
            list[n++] = p;
        }
    }

    fclose(f);
    *count = n;
    return list;
}

// Print Gantt chart + metrics
void print_results(Process *p, int n, Gantt *g, int gc) {
    printf("\nGantt Chart:\n");
    for (int i = 0; i < gc; i++) printf("| %s ", g[i].label);
    printf("|\n");
    for (int i = 0; i < gc; i++) printf("%-5d", g[i].start);
    printf("%-5d\n", g[gc-1].end);

    printf("\nPID  Arrival  Burst  Completion  Turnaround  Waiting\n");
    double total_wt = 0, total_tat = 0;
    for (int i = 0; i < n; i++) {
        int tat = p[i].completion - p[i].arrival;
        int wt = tat - p[i].burst;
        printf("%-4d %-8d %-6d %-11d %-11d %-6d\n", p[i].pid, p[i].arrival, p[i].burst, p[i].completion, tat, wt);
        total_wt += wt;
        total_tat += tat;
    }

    printf("\nAverage Waiting Time: %.2f\n", total_wt / n);
    printf("Average Turnaround Time: %.2f\n", total_tat / n);
}

// FCFS scheduling
void fcfs(Process *p, int n) {
    printf("\n--- FCFS Scheduling ---\n");
    // Sort by arrival time
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (p[j].arrival < p[i].arrival) {
                Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }

    Gantt gantt[100];
    int time = 0, gc = 0;
    for (int i = 0; i < n; i++) {
        if (time < p[i].arrival) time = p[i].arrival;
        p[i].start_time = time;
        time += p[i].burst;
        p[i].completion = time;
        sprintf(gantt[gc].label, "P%d", p[i].pid);
        gantt[gc].start = p[i].start_time;
        gantt[gc].end = p[i].completion;
        gc++;
    }
    print_results(p, n, gantt, gc);
}

// SJF scheduling (non-preemptive)
void sjf(Process *p, int n) {
    printf("\n--- SJF Scheduling ---\n");
    // Sort by arrival first
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (p[j].arrival < p[i].arrival) {
                Process temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }

    int completed = 0, time = 0;
    int visited[100] = {0};
    Gantt gantt[100];
    int gc = 0;

    while (completed < n) {
        int idx = -1;
        int min_burst = 9999;
        for (int i = 0; i < n; i++) {
            if (!visited[i] && p[i].arrival <= time && p[i].burst < min_burst) {
                min_burst = p[i].burst;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        visited[idx] = 1;
        if (time < p[idx].arrival) time = p[idx].arrival;
        p[idx].start_time = time;
        time += p[idx].burst;
        p[idx].completion = time;
        sprintf(gantt[gc].label, "P%d", p[idx].pid);
        gantt[gc].start = p[idx].start_time;
        gantt[gc].end = p[idx].completion;
        gc++;
        completed++;
    }
    print_results(p, n, gantt, gc);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s processes.txt\n", argv[0]);
        return 1;
    }

    int n;
    Process *procs = read_processes(argv[1], &n);

    printf("Read %d processes from %s\n", n, argv[1]);
    printf("\nChoose scheduling algorithm:\n");
    printf("1) First Come First Serve (FCFS)\n");
    printf("2) Shortest Job First (SJF)\n");
    printf("Choice: ");
    int choice;
    scanf("%d", &choice);

    if (choice == 1) {
        fcfs(procs, n);
    } else if (choice == 2) {
        sjf(procs, n);
    } else {
        printf("Invalid choice.\n");
    }

    free(procs);
    return 0;
}

