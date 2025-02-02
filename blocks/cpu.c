#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 256

// Function to retrieve the CPU usage statistics from /proc/stat
void get_cpu_usage(float *user, float *system, float *idle) {
    FILE *fp;
    char buffer[BUFFER_SIZE];
    unsigned long u, s, i, ni, io, irq, softirq, steal;

    fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        perror("Error opening /proc/stat");
        return;
    }

    // Read the first line from /proc/stat which contains CPU times
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        perror("Error reading from /proc/stat");
        fclose(fp);
        return;
    }

    // Parse the line containing CPU usage stats
    if (sscanf(buffer, "cpu %lu %lu %lu %lu %lu %lu %lu %lu", 
                &u, &ni, &s, &i, &io, &irq, &softirq, &steal) != 8) {
        perror("Error parsing CPU stats");
        fclose(fp);
        return;
    }

    // Assign values to the passed pointers
    *user = (float) u;
    *system = (float) s;
    *idle = (float) i;

    fclose(fp);
}

// Function to calculate the CPU usage percentage based on two readings
float calculate_cpu_usage(float prev_user, float prev_system, float prev_idle,
                          float curr_user, float curr_system, float curr_idle) {
    float total_prev = prev_user + prev_system + prev_idle;
    float total_curr = curr_user + curr_system + curr_idle;
    float total_diff = total_curr - total_prev;
    float usage_diff = (curr_user + curr_system - prev_user - prev_system);

    // Ensure we don’t divide by zero if no change in CPU times occurs
    if (total_diff == 0) {
        return 0.0f;  // No CPU activity change
    }

    // Return the CPU usage as a percentage
    return (usage_diff / total_diff) * 100.0f;
}

// Function to get the CPU usage in a period
float get_cpu_usage_percentage() {
    float prev_user, prev_system, prev_idle;
    float curr_user, curr_system, curr_idle;
    float cpu_usage;

    // Get initial CPU statistics
    get_cpu_usage(&prev_user, &prev_system, &prev_idle);

    // Sleep for 1 second to simulate measuring over time
    sleep(1);

    // Get updated CPU statistics
    get_cpu_usage(&curr_user, &curr_system, &curr_idle);

    // Calculate the CPU usage percentage
    cpu_usage = calculate_cpu_usage(prev_user, prev_system, prev_idle, 
                                    curr_user, curr_system, curr_idle);

    return cpu_usage;
}

int main() {
    float cpu_usage = get_cpu_usage_percentage();
    
    // Output the CPU usage as a percentage
    printf("%.1f%%", cpu_usage);

    return 0;
}

