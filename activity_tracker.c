// Include necessary libraries for system calls, process management, and time handling
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <windows.h>
#include <tlhelp32.h>
#include <time.h>
#include <psapi.h>
#include <signal.h>
#pragma comment(lib, "psapi.lib")  // Link to psapi.lib for process management

// Define a structure to store activity data
typedef struct Activity
{   
    char process_path[256];  // Path to the executable file of the process
    char title[256];         // Window title of the active process
    char name[256];          // Process name
    int time;                // Time spent on the process (in seconds)
    struct Activity *next;   // Pointer to the next activity in the list
} Activity;

// Head pointer to the linked list of activities
Activity* head = NULL;

// Function to insert a new activity or update the time for an existing activity
void insert_activity(char *path, char *name, int time, char *title) {
    Activity *current = head, *previous = NULL;  

    // Traverse the linked list to find an activity with the same process path
    while(current != NULL && (strcmp(current->process_path, path) != 0)) {
        previous = current;
        current = current->next;
    }

    // If activity is found, update the time
    if(current != NULL) {
        current->time += 1;  // Increment the time spent on this activity
    } else {
        // Otherwise, create a new activity
        Activity* newActivity = (Activity*)malloc(sizeof(Activity));
        strncpy(newActivity->process_path, path, sizeof(newActivity->process_path) - 1);
        strncpy(newActivity->name, name, sizeof(newActivity->name) - 1);
        strncpy(newActivity->title, title, sizeof(newActivity->title) - 1);  
        newActivity->time = time;
        newActivity->next = NULL;

        // Insert the new activity at the beginning or end of the list
        if(previous == NULL) {
            head = newActivity;
        } else {
            previous->next = newActivity;
        }
    }
}

// Function to get the process name from the process ID
BOOL GetProcessNameFromID(DWORD processID, char* processName, DWORD size) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            // If the process ID matches, copy the process name
            if (pe32.th32ProcessID == processID) {
                strncpy(processName, pe32.szExeFile, size);
                processName[size - 1] = '\0'; 
                CloseHandle(hProcessSnap);
                return TRUE;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return FALSE;
}

// Function to process and sort activities based on specific rules
void sort_activities(char *path, char *name, int time, char *title) {
    char process_name[64];
    strcpy(process_name, name);

    // Special case for certain processes like ApplicationFrameHost
    if (strcmp(process_name, "ApplicationFrameHost.exe") == 1) {
        strcpy(process_name, title);
        insert_activity(path, process_name, time, title);
    }
    // Handle Explorer with no title
    else if (strcmp(process_name, "explorer.exe") == 1 && strcmp(title, "None") == 1) {
        strcpy(process_name, title);
        insert_activity(path, process_name, 0, title);
    } 
    else {
        //insert the activity as usual
        insert_activity(path, process_name, time, title);
    }
}

// Function to get today's date in the format yyyy-mm-dd.txt
char* get_todays_date() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    static char date[12]; // yyyy-mm-dd + null terminator
    sprintf(date, "%04d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    return date;
}
//Function to clear the linked list of activities to avoid memory leaks
void free_activities() {
    Activity *current = head;
    while (current != NULL) {
        Activity *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}


// Function to save the list of activities to a file
void save_activities_to_file(const char *filename) {
    FILE *fp = fopen(filename, "w+");
    if (fp == NULL) {
        perror("Error opening file");
        free_activities();
        return;
    }

    Activity *current = head;
    while (current != NULL) {
        // Write each activity to the file with its details
        fprintf(fp, "%s||||%s||||%d||||%s\n", current->process_path, current->name, current->time, current->title);
        current = current->next;
    }
    free_activities();
    fflush(fp);

    fclose(fp);
}

// Function to load activities from a file
int load_activities_from_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 80;
    }

    char line[1024];
    while (fgets(line, sizeof(line), fp)) {
        char path[256], title[256], name[256];
        int time;
        // Read each line and extract the data for each activity
        if (sscanf(line, "%255[^|]|%*3[|]%255[^|]|%*3[|]%d|%*3[|]%255[^\n]", path, name, &time, title) == 4) {
            insert_activity(path, name, time, title);
        } else {
            fprintf(stderr, "Error parsing line: %s", line);
        }
    }

    fclose(fp);
}

// Function to get the window title of a specific window handle
BOOL GetWindowTitle(HWND hwnd, char* title, DWORD size) {
    return GetWindowTextA(hwnd, title, size) > 0;
}

// Main function to track activities continuously
void main() {
    // Load activities from file, or create an empty file if not found
    char *file_name = get_todays_date();
    if(load_activities_from_file(file_name) == 80) {
        FILE *f = fopen(file_name, "w");
        fclose(f);
        int return_value = load_activities_from_file("data.txt");
    }

    int i = 0;
    while (TRUE) {
        Sleep(3000);  // Sleep for 3 seconds between iterations

        HWND hWnd = GetForegroundWindow();  // Get the current foreground window
        if (hWnd) {
            DWORD processId;
            GetWindowThreadProcessId(hWnd, &processId);  // Get process ID of the window

            char processName[MAX_PATH] = "Unknown";
            char filePath[MAX_PATH] = "Unknown";
            char windowTitle[MAX_PATH] = "None";

            // Open the process to query information
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
            if (hProcess != NULL) {
                // Get the file path of the running process
                if (GetModuleFileNameExA(hProcess, NULL, filePath, MAX_PATH) == 0) {
                    DWORD error = GetLastError();
                    if (error == ERROR_ACCESS_DENIED) {
                        strcpy(filePath, "Access Denied");
                    } else {
                        sprintf(filePath, "Error: %lu", error);  
                    }
                }

                // Get the process name from its ID
                if (!GetProcessNameFromID(processId, processName, sizeof(processName))) {
                    strcpy(processName, "Unknown");
                }
                
                // Get the title of the active window
                if (GetWindowTitle(hWnd, windowTitle, sizeof(windowTitle))) {
                    printf("Window Title: %s\n", windowTitle);
                }

                CloseHandle(hProcess);
            } else {
                DWORD error = GetLastError();
                sprintf(filePath, "Failed to open process. Error: %lu", error);
            }

            // Print process details to console
            printf("Active window: %s\n", processName);
            printf("Process file path: %s\n", filePath);
            fflush(stdout);

            // Log activity and process it
            char logEntry[MAX_PATH * 3];
            snprintf(logEntry, sizeof(logEntry), "%s (%s): %s\n", processName, windowTitle, filePath);
            sort_activities(filePath, processName,  1, windowTitle);
        }

        i++;
        if(i > 20) {
            // Save the activities to file every 20 iterations
            char *file_name = get_todays_date();
            save_activities_to_file(file_name);
            i = 0;
        }
    }
}  
