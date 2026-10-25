# Tutorial 9: Build a Complete Local Task Tracker

This tutorial starts with an empty VNT source file and builds a usable command-line application. It stores tasks in a plain-text file in the current working directory. It does not require a database, network connection, or external runtime. The complete runnable version is also available at [examples/task_tracker.vnt](../../examples/task_tracker.vnt); the steps below build it up in stages.

## 1. Create the project

Create **task_tracker.vnt** in the repository root. Keeping it there makes the imports below resolve as written.

Start with the imports and persistent file path:

```vnt
import "lib/std/fs.vnt" as fs
import "lib/std/log.vnt" as log

tasks_path = "vnt_tasks.txt"
running = true
```

The task data is kept in **vnt_tasks.txt**. The log module appends diagnostic records to **vnt.log**. Both paths are relative to the directory where you launch the executable.

## 2. Add a task

Add this function below the variables:

```vnt
fun add_task() {
    task = input("Task (blank cancels): ")

    if task == "" {
        print("Cancelled: task was empty.")
        return 0
    }

    saved = fs.append_text(tasks_path, "- " + task + "\n")

    if saved {
        print("Task saved.")
        logged = log.info("Task added.")
    } else {
        print("ERROR: Could not save the task. Check the folder permissions.")
        logged = log.error("Could not append to the task file.")
    }

    return 0
}
```

The filesystem call returns a boolean. The program checks it instead of reporting success unconditionally. This is VNT's usual error-handling style for these APIs: inspect return values and, for networking, inspect the status helper. VNT does not currently provide general exception handling.

## 3. List saved tasks

Add a function to display the stored file:

```vnt
fun list_tasks() {
    if !fs.exists(tasks_path) {
        print("No tasks yet. Choose Add task first.")
        return 0
    }

    contents = fs.read_text(tasks_path)

    if contents == "" {
        print("The task list is empty.")
    } else {
        print("===== SAVED TASKS =====")
        print(contents)
        print("=======================")
    }

    return 0
}
```

The file is plain text, with one task per line. This keeps the project easy to inspect and back up. The standard filesystem module is text-oriented, not a general binary-file API.

## 4. Add a safe clear operation

Add a confirmation before deleting the task file:

```vnt
fun clear_tasks() {
    if !fs.exists(tasks_path) {
        print("There is no task file to clear.")
        return 0
    }

    confirmation = input("Type CLEAR to delete every task: ")

    if confirmation != "CLEAR" {
        print("Cancelled. Tasks were not changed.")
        return 0
    }

    if fs.remove_file(tasks_path) {
        print("All tasks were deleted.")
        logged = log.warn("The task file was cleared.")
    } else {
        print("ERROR: Could not delete the task file.")
        logged = log.error("Could not remove the task file.")
    }

    return 0
}
```

The confirmation is intentionally exact. Do not remove it unless you also change how destructive actions are communicated to the user.

## 5. Add the application loop

Finish the file with this menu:

```vnt
while running {
    print("")
    print("========== VNT TASK TRACKER ==========")
    print("1. Add task")
    print("2. List tasks")
    print("3. Clear all tasks")
    print("4. Exit")

    choice = input("task-tracker> ")

    if choice == "1" {
        operation_result = add_task()
    } else if choice == "2" {
        operation_result = list_tasks()
    } else if choice == "3" {
        operation_result = clear_tasks()
    } else if choice == "4" {
        running = false
        print("Exiting. Saved tasks remain on disk.")
    } else {
        print("Unknown option. Choose 1, 2, 3, or 4.")
    }
}
```

The complete file consists of the imports and variables from step 1, followed by the three functions, then the loop above. Function declarations should appear before the statements that call them.

## 6. Build and run it

From the repository root on Windows, with VNT already built:

```powershell
.\vnt.exe --compile task_tracker.vnt -o task_tracker.exe
.\task_tracker.exe
```

Try this sequence:

1. Choose **1** and add “Review the VNT compiler pipeline”.
2. Choose **2** and confirm the task appears.
3. Exit and launch the program again; the task should still be present.
4. Choose **3**, enter something other than CLEAR, and verify the task remains.
5. Choose **3** again and enter CLEAR; the task file should be removed.
6. Choose **2** to confirm the program handles the missing file.

## 7. Project structure and next improvements

For a tiny program, one source file is enough:

```text
VNT/
├── task_tracker.vnt
├── vnt_tasks.txt     # created when the first task is saved
└── vnt.log           # created when the log module writes a record
```

Useful extensions include task completion markers, a separate configuration file, timestamps from the time module, or moving reusable task operations into an exported module. Keep file writes checked, validate menu choices, and avoid silently deleting user data.

Compile after changes and run the regression suite from the repository root:

```powershell
.\tests\run_language_checks.ps1
```

That suite checks compiler and language behavior; it does not replace manually trying the application's success, cancellation, and file-error paths.

Next: [Return to the tutorial index](README.md).
