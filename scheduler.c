// author: Cristofer Herrera-Mejia and Hernan Cortes

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int waiting_time;
    int turnaround_time;
    int completion_time;
    int executed; // needed this for SJF
} Process;

// Function prototypes
void read_processes_from_file(const char* filename, Process processes[], int* count);
void fcfs_scheduling(Process processes[], int count);
void sjf_scheduling(Process processes[], int count);
void print_gantt_chart(int pids[], int times[], int count);
void print_metrics(Process processes[], int count);
void sort_by_arrival_time(Process processes[], int count);
void reset_processes(Process processes[], int count);

int main() {
    Process processes[MAX_PROCESSES];
    int process_count = 0;
    int choice;
    
    printf("=== CPU Scheduling Simulator ===\n");
    
    // Read processes from file
    read_processes_from_file("processes.txt", processes, &process_count);
    
    if (process_count == 0) {
        printf("No processes found or error reading file.\n");
        return 1;
    }
    
    printf("Successfully read %d processes from file.\n\n", process_count);
    
    do {
        printf("\n=== Scheduling Algorithms ===\n");
        printf("1. First-Come, First-Served (FCFS)\n");
        printf("2. Shortest Job First (SJF)\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                printf("\n--- FCFS Scheduling ---\n");
                fcfs_scheduling(processes, process_count);
                break;
            case 2:
                printf("\n--- SJF Scheduling ---\n");
                sjf_scheduling(processes, process_count);
                break;
            case 3:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
        
        // Reset for next run
        reset_processes(processes, process_count);
        
    } while(choice != 3);
    
    return 0;
}

void read_processes_from_file(const char* filename, Process processes[], int* count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        *count = 0;
        return;
    }
    
    char line[256];
    int line_num = 0;
    *count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        
        // skip header line
        if (line_num == 1 || strlen(line) <= 1) {
            continue;
        }
        
        // parse the data
        if (sscanf(line, "%d %d %d %d", 
                   &processes[*count].pid,
                   &processes[*count].arrival_time,
                   &processes[*count].burst_time,
                   &processes[*count].priority) == 4) {
            
            // initialize everything to 0
            processes[*count].waiting_time = 0;
            processes[*count].turnaround_time = 0;
            processes[*count].completion_time = 0;
            processes[*count].executed = 0;
            
            (*count)++;
            
            if (*count >= MAX_PROCESSES) {
                printf("Warning: Maximum process limit reached (%d)\n", MAX_PROCESSES);
                break;
            }
        }
    }
    
    fclose(file);
}

void fcfs_scheduling(Process processes[], int count) {
    int gantt_pids[MAX_PROCESSES * 2];
    int gantt_times[MAX_PROCESSES * 2 + 1];
    int gantt_count = 0;
    
    // sort by arrival first
    sort_by_arrival_time(processes, count);
    
    int current_time = 0;
    gantt_times[gantt_count] = current_time;
    
    printf("Execution Order: ");
    
    for (int i = 0; i < count; i++) {
        // if CPU is idle, jump to next arrival
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        
        gantt_pids[gantt_count] = processes[i].pid;
        gantt_count++;
        
        printf("P%d ", processes[i].pid);
        
        // do the calculations
        processes[i].completion_time = current_time + processes[i].burst_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        
        current_time = processes[i].completion_time;
        gantt_times[gantt_count] = current_time;
    }
    
    printf("\n\n");
    
    print_gantt_chart(gantt_pids, gantt_times, gantt_count);
    print_metrics(processes, count);
}

void sjf_scheduling(Process processes[], int count) {
    int gantt_pids[MAX_PROCESSES * 2];
    int gantt_times[MAX_PROCESSES * 2 + 1];
    int gantt_count = 0;
    
    int current_time = 0;
    int completed = 0;
    gantt_times[gantt_count] = current_time;
    
    printf("Execution Order: ");
    
    while (completed < count) {
        int shortest_idx = -1;
        int shortest_burst = 999999;
        
        // find shortest job that's arrived and not done yet
        for (int i = 0; i < count; i++) {
            if (!processes[i].executed && 
                processes[i].arrival_time <= current_time && 
                processes[i].burst_time < shortest_burst) {
                shortest_burst = processes[i].burst_time;
                shortest_idx = i;
            }
        }
        
        // nothing ready? skip to next arrival
        if (shortest_idx == -1) {
            int next_arrival = 999999;
            for (int i = 0; i < count; i++) {
                if (!processes[i].executed && processes[i].arrival_time < next_arrival) {
                    next_arrival = processes[i].arrival_time;
                }
            }
            current_time = next_arrival;
            continue;
        }
        
        // run the shortest one
        processes[shortest_idx].executed = 1;
        completed++;
        
        gantt_pids[gantt_count] = processes[shortest_idx].pid;
        gantt_count++;
        
        printf("P%d ", processes[shortest_idx].pid);
        
        // calculate times
        processes[shortest_idx].completion_time = current_time + processes[shortest_idx].burst_time;
        processes[shortest_idx].turnaround_time = processes[shortest_idx].completion_time - 
                                                   processes[shortest_idx].arrival_time;
        processes[shortest_idx].waiting_time = processes[shortest_idx].turnaround_time - 
                                                processes[shortest_idx].burst_time;
        
        current_time = processes[shortest_idx].completion_time;
        gantt_times[gantt_count] = current_time;
    }
    
    printf("\n\n");
    
    print_gantt_chart(gantt_pids, gantt_times, gantt_count);
    print_metrics(processes, count);
}

void print_gantt_chart(int pids[], int times[], int count) {
    printf("Gantt Chart:\n");
    
    printf(" ");
    for (int i = 0; i < count; i++) {
        printf("P%-3d", pids[i]);
        if (i < count - 1) printf(" ");
    }
    printf("\n");
    
    for (int i = 0; i <= count; i++) {
        printf("%-4d", times[i]);
    }
    printf("\n\n");
}

void print_metrics(Process processes[], int count) {
    float avg_waiting = 0, avg_turnaround = 0;
    
    printf("Process Metrics:\n");
    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\tCompletion\n");
    printf("---\t-------\t-----\t-------\t----------\t----------\n");
    
    for (int i = 0; i < count; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t\t%d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].waiting_time,
               processes[i].turnaround_time,
               processes[i].completion_time);
        
        avg_waiting += processes[i].waiting_time;
        avg_turnaround += processes[i].turnaround_time;
    }
    
    avg_waiting /= count;
    avg_turnaround /= count;
    
    printf("\nAverage Waiting Time: %.2f\n", avg_waiting);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround);
    
    // CPU utilization calc
    int total_burst = 0;
    int last_completion = 0;
    for (int i = 0; i < count; i++) {
        total_burst += processes[i].burst_time;
        if (processes[i].completion_time > last_completion) {
            last_completion = processes[i].completion_time;
        }
    }
    
    float cpu_util = (float)total_burst / last_completion * 100;
    printf("CPU Utilization: %.2f%%\n", cpu_util);
}

void sort_by_arrival_time(Process processes[], int count) {
    // bubble sort - yeah i know its not the fastest but it works
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

void reset_processes(Process processes[], int count) {
    for (int i = 0; i < count; i++) {
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].completion_time = 0;
        processes[i].executed = 0;
    }
}