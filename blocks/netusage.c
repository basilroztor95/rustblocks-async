#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUF_SIZE 255
#define KB 1024
#define MB (1024 * KB)
#define NETWORK_FILE "/proc/net/dev"

// Global variables for tracking received and sent data
unsigned long long total_received = 0, total_sent = 0;

// Function to open and parse /proc/net/dev for network statistics
static int open_and_parse_netdev(FILE **dev_file) {
    *dev_file = fopen(NETWORK_FILE, "r");
    if (!*dev_file) {
        perror("Error opening " NETWORK_FILE);
        return -1;
    }
    return 0;
}

// Function to skip the first two header lines in /proc/net/dev
static void skip_header_lines(FILE *dev_file) {
    char buffer[BUF_SIZE];
    for (int i = 0; i < 2; i++) {
        if (!fgets(buffer, BUF_SIZE, dev_file)) {
            perror("Error skipping header line");
            return;
        }
    }
}

// Function to parse a network interface's stats (received/sent bytes)
static int parse_interface_data(FILE *dev_file, const char *interface, unsigned long long *received, unsigned long long *sent) {
    char buffer[BUF_SIZE];

    // Read the file line by line to find the matching interface
    while (fgets(buffer, BUF_SIZE, dev_file)) {
        // If the line contains the interface name, extract received/sent bytes
        if (strstr(buffer, interface)) {
            if (sscanf(strchr(buffer, ':') + 1, "%llu %*d %*d %*d %*d %*d %*d %*d %llu", received, sent) == 2) {
                return 0; // Successfully parsed the data
            }
        }
    }
    return -1; // Interface not found
}

// Function to calculate speed (in KB or MB) based on the difference between new and old values
static void calculate_speed(char *result_str, unsigned long long new_value, unsigned long long old_value) {
    double speed_in_kb = (new_value - old_value) / (double)KB;

    // Determine whether to display the result in KB or MB
    if (speed_in_kb > KB) {
        snprintf(result_str, 15, "%.1f MB", speed_in_kb / (double)KB);
    } else {
        snprintf(result_str, 15, "%.1f KB", speed_in_kb);
    }
}

// Function to monitor network speed for a given interface
static int monitor_network_speed(const char *interface) {
    char result_str[42], download_speed[15], upload_speed[15];
    unsigned long long current_received, current_sent;
    FILE *dev_file = NULL;

    // Open and parse /proc/net/dev
    if (open_and_parse_netdev(&dev_file) < 0) {
        return -1; // Return error if failed to open /proc/net/dev
    }

    // Skip the header lines
    skip_header_lines(dev_file);

    // Parse the initial data for the interface
    if (parse_interface_data(dev_file, interface, &total_received, &total_sent) < 0) {
        fclose(dev_file);
        return -1; // Return error if the interface data can't be found
    }

    // Sleep for 1 second to calculate the change in network usage
    sleep(1);

    // Reopen and parse the file again after 1 second
    if (open_and_parse_netdev(&dev_file) < 0) {
        return -1;
    }

    // Skip the header lines again
    skip_header_lines(dev_file);

    // Parse the new data after the sleep period
    if (parse_interface_data(dev_file, interface, &current_received, &current_sent) < 0) {
        fclose(dev_file);
        return -1; // Return error if data is not found
    }

    // Calculate download and upload speeds
    calculate_speed(download_speed, current_received, total_received);
    calculate_speed(upload_speed, current_sent, total_sent);

    // Prepare the final result string
    snprintf(result_str, sizeof(result_str), "%s %s", download_speed, upload_speed);

    // Update global counters with the latest data
    total_received = current_received;
    total_sent = current_sent;

    // Print the result to the terminal
    printf("%s", result_str);

    // Close the file
    fclose(dev_file);

    return 0; // Success
}

int main(void) {
    const char *interface = "enp5s0";  // Name of the network interface to monitor

    // Start monitoring the network speed for the specified interface
    if (monitor_network_speed(interface) < 0) {
        return EXIT_FAILURE; // Exit if monitoring fails
    }

    return EXIT_SUCCESS; // Successful execution
}

