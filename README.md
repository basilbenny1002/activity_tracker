
# Activity Tracker

## Overview
The **Activity Tracker** is a lightweight C program designed to monitor the time you spend on various applications on your Windows system. It logs details like the window title, file path of the active process, and the total time spent on each application. The program outputs the log to a file named with the current date (e.g., `YYYY-MM-DD.txt`).

### Key Features
- Tracks active window details such as:
  - **Window Title**
  - **Process File Path**
  - **Total Time Spent** (in seconds, calculated as the logged value divided by 2 since updates occur every 0.5 seconds).
- Logs activity continuously and updates the file without replacing existing values.
- Automatically adds to the previous session's values when the program is rerun.
- Outputs data in the format:
  ```
  <process_path>||||<process_name>||||<time_spent_in_seconds>||||<window_title>
  ```
- Designed to run in the background continuously.
- The file is updated every 20 iterations (approximately 30 seconds).

### Upcoming Features
- A graphical user interface (GUI) is in development to visualize the tracked data.
- The GUI will provide better insights, such as a pie chart of time spent on different applications.

## Requirements
- **Windows Operating System**
- Compiler with support for `psapi.lib`
  - Ensure you compile the code with the `-lpsapi` flag to link the required library.

## Usage

### Using the Makefile
1. Open a terminal or command prompt in the directory containing the `activity_tracker.c` file and the `Makefile`.
2. To compile the program, run:
   ```bash
   make
   ```
3. This will create an executable named `activity_tracker.exe` in the same directory.
4. To remove the compiled executable, use:
   ```bash
   make clean
   ```

### Manual Compilation
If you choose not to use the Makefile:
1. Compile the script using a C compiler:
   ```bash
   gcc activity_tracker.c -o activity_tracker.exe -lpsapi
   ```
2. Run the compiled binary:
   ```bash
   activity_tracker.exe
   ```

### Running the Program
- When executed, the program logs activity data to a file named `YYYY-MM-DD.txt` in the same directory as the executable.

## Output File Details
- Each day's log is stored in a file named after the current date (`YYYY-MM-DD.txt`).
- If the file already exists, the program appends the new session's data to the existing log.

## How It Works
- The script captures the currently active window's process details every 0.5 seconds.
- Data is saved every 30 seconds (20 iterations).
- Total time spent on each application is updated continuously during runtime.

## Future Enhancements
- GUI for real-time data visualization.
- Options for exporting data in additional formats.
- Configuration options for update intervals and logging preferences.

## Files Included
1. **`activity_tracker.c`**: The main script file.
2. **`README.md`**: Project documentation (this file).
3. **`Makefile`** (optional): Compilation instructions.
4. **`2024-12-31.txt`** (optional): A sample `YYYY-MM-DD.txt` file to demonstrate the output format.
