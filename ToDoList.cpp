#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cstdlib>
using namespace std;

/*
    @class Task
    @brief Represents a task with various attributes such as name, status, priority, due date, categories, reminder date, dependencies, and notes.

    The Task class is used to encapsulate the details of a task in a to-do list application. It includes attributes for the task's name, status, priority, due date, categories, reminder date, dependencies, and notes.
    
    @var string Task::name
    The name of the task.
    
    @var string Task::status
    The status of the task (e.g., "Incomplete", "Complete").
    
    @var int Task::priority
    The priority level of the task.
    
    @var string Task::dueDate
    The due date of the task.
    
    @var vector<string> Task::categories
    The categories associated with the task.
    
    @var string Task::reminderDate
    The reminder date for the task.
    
    @var unordered_set<int> Task::dependencies
    The set of task IDs that this task depends on.
    
    @var string Task::notes
    Additional notes for the task.
    
    @fn Task::Task(const string& taskName, const string& taskStatus = "Incomplete")
    @brief Constructs a new Task object with the given name and status.
    
    @param taskName The name of the task.
    @param taskStatus The status of the task, defaulting to "Incomplete".
*/
class Task {
public:
    string name;
    string status;
    int priority;
    string dueDate;
    vector<string> categories;
    string reminderDate;
    unordered_set<int> dependencies;
    string notes;

    Task(const string& taskName, const string& taskStatus = "Incomplete")
        : name(taskName), status(taskStatus), priority(0) {}
};

class ToDoList {
private:
    vector<Task> tasks;
    stack<pair<string, Task>> undoStack;
    stack<pair<string, Task>> redoStack;
    priority_queue<pair<int, int>> priorityQueue;
    unordered_map<string, int> categoryCount;
   void addDependency(int dependentIndex, int dependencyIndex) {
        if (dependentIndex >= 0 && dependentIndex < tasks.size() && dependencyIndex >= 0 && dependencyIndex < tasks.size()) {
            tasks[dependentIndex].dependencies.insert(dependencyIndex);
            cout << "Task \"" << tasks[dependentIndex].name << "\" now depends on task \"" << tasks[dependencyIndex].name << "\"." << endl;
        } else {
            cout << "Invalid task indices. Please provide valid task indices." << endl;
        }
    }
public:
    void addTask(const string& taskName) {
        Task newTask(taskName);
        tasks.push_back(newTask);
        undoStack.push({"add", newTask});
        redoStack = stack<pair<string, Task>>();

        // Update priority queue and category count
        priorityQueue.push({0, tasks.size() - 1});
        updateCategoryCount(newTask);
    }

    void removeTask(int taskIndex) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            Task removedTask = tasks[taskIndex];
            tasks.erase(tasks.begin() + taskIndex);
            undoStack.push({"remove", removedTask});
            redoStack = stack<pair<string, Task>>();

            // Update category count after removal
            updateCategoryCount(removedTask, true);
        }
    }

    void markComplete(int taskIndex) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].status = "Complete";
            undoStack.push({"mark_incomplete", tasks[taskIndex]});
            redoStack = stack<pair<string, Task>>();
        }
    }

    void setTaskPriority(int taskIndex, int priority) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            int oldPriority = tasks[taskIndex].priority;
            tasks[taskIndex].priority = priority;

            // Update priority queue if priority is changed
            if (oldPriority != priority) {
                priorityQueue.push({priority, taskIndex});
            }
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }

    void setTaskDueDate(int taskIndex, const string& dueDate) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].dueDate = dueDate;
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }

    void addTaskCategory(int taskIndex, const string& category) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].categories.push_back(category);
            updateCategoryCount(tasks[taskIndex]);
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }

    void setReminder(int taskIndex, const string& reminderDate) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].reminderDate = reminderDate;
            cout << "Reminder set for task \"" << tasks[taskIndex].name << "\" on " << reminderDate << endl;
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }


    void updateCategoryCount(const Task& task, bool decrement = false) {
        for (const string& category : task.categories) {
            if (decrement) {
                categoryCount[category]--;
            } else {
                categoryCount[category]++;
            }
        }
    }

    void displayCategoryCount() {
        cout << "Category Count:" << endl;
        for (const auto& entry : categoryCount) {
            cout << entry.first << ": " << entry.second << " tasks" << endl;
        }
    }

    void undo() {
        if (!undoStack.empty()) {
            auto action = undoStack.top();
            undoStack.pop();

            if (action.first == "add") {
                tasks.pop_back();
                priorityQueue.pop();  // Remove from priority queue
                updateCategoryCount(action.second, true);
            } else if (action.first == "remove") {
                tasks.push_back(action.second);
                priorityQueue.push({tasks.back().priority, tasks.size() - 1});
                updateCategoryCount(action.second);
            } else if (action.first == "mark_incomplete") {
                action.second.status = "Incomplete";
            }

            redoStack.push(action);
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            auto action = redoStack.top();
            redoStack.pop();

            if (action.first == "add") {
                tasks.push_back(action.second);
                priorityQueue.push({tasks.back().priority, tasks.size() - 1});
                updateCategoryCount(action.second);
            } else if (action.first == "remove") {
                tasks.pop_back();
                priorityQueue.pop();  // Remove from priority queue
                updateCategoryCount(action.second, true);
            } else if (action.first == "mark_incomplete") {
                action.second.status = "Complete";
            }

            undoStack.push(action);
        }
    }

    void displayTasks() {
        for (size_t i = 0; i < tasks.size(); ++i) {
            cout << i + 1 << ". [" << tasks[i].status << "] " << tasks[i].name << endl;
            cout << "   Priority: " << tasks[i].priority << endl;
            cout << "   Due Date: " << tasks[i].dueDate << endl;
            cout << "   Notes:" << tasks[i].notes   << endl;                                     //update
            cout << "   Categories: ";
            for (const string& category : tasks[i].categories) {
                cout << category << ", ";
            }
            cout << "   Reminder: " << tasks[i].reminderDate << endl;
            cout << endl;
        }
    }

void setTaskDependencies(int taskIndex) {
    if (taskIndex >= 0 && taskIndex < tasks.size()) {
        cout << "Enter indices of tasks that " << tasks[taskIndex].name << " depends on (-1 to stop): ";

        // Clear existing dependencies for the task
        tasks[taskIndex].dependencies.clear();

        int dependencyIndex;
        while (true) {
            cin >> dependencyIndex;
            cin.ignore();                                                                                      //update
            if (dependencyIndex == -1) {
                break;
            }
            if (dependencyIndex >= 0 && dependencyIndex <=tasks.size()) {                                      //update
                addDependency(taskIndex, dependencyIndex-1);
            } else {
                cout << "Invalid task index. Please provide a valid task index or -1 to stop." << endl;
            }
        }
    } else {
        cout << "Invalid task index. Please provide a valid task index." << endl;
    }
}
void displayTaskDependencies(int taskIndex) {
    if (taskIndex >= 0 && taskIndex < tasks.size()) {
        const Task& task = tasks[taskIndex];
        cout << "Dependencies for task \"" << task.name << "\": " << endl;
        for (int dependencyIndex : task.dependencies) {
                if((tasks[dependencyIndex].name).length()!=0)                                               //update
            cout << "- " << tasks[dependencyIndex].name << endl;
        }
    } else {
        cout << "Invalid task index. Please provide a valid task index." << endl;
    }
}

    void visualizeTaskDependencies() {
        cout << "Task Dependencies Visualization:" << endl;
        for (size_t i = 0; i < tasks.size(); ++i) {
            cout << "Task \"" << tasks[i].name << "\" depends on: ";
            for (int dependencyIndex : tasks[i].dependencies) {
                cout << tasks[dependencyIndex].name << ", ";
            }
            cout << endl;
        }
    }

    void removeDependency(int dependentIndex, int dependencyIndex) {
        if (dependentIndex >= 0 && dependentIndex < tasks.size() && dependencyIndex >= 0 && dependencyIndex < tasks.size()) {
            tasks[dependentIndex].dependencies.erase(dependencyIndex);
        } else {
            cout << "Invalid task indices. Please provide valid task indices." << endl;
        }
    }

    void updateDependencies(int taskIndex) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].dependencies.clear();
            cout << "Enter updated indices of tasks that " << tasks[taskIndex].name << " depends on (-1 to stop): ";
            int dependencyIndex;
            while (true) {
                cin >> dependencyIndex;
                if (dependencyIndex == -1) {
                    break;
                }
                if (dependencyIndex >= 0 && dependencyIndex <= tasks.size()) {
                    addDependency(taskIndex, dependencyIndex-1);                                                //update
                } else {
                    cout << "Invalid task index. Please provide a valid task index or -1 to stop." << endl;
                }
            }
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }

    void taskSearch(const string& keyword) {
        cout << "Search results for keyword \"" << keyword << "\":" << endl;
        for (size_t i = 0; i < tasks.size(); ++i) {
            if (tasks[i].name.find(keyword) != string::npos || tasks[i].notes.find(keyword) != string::npos) {
                cout << i + 1 << ". " << tasks[i].name << endl;
            }
        }
    }

    void displayStatistics() {
        int totalTasks = tasks.size();
        int completedTasks = 0;
        for (const Task& task : tasks) {
            if (task.status == "Complete") {
                completedTasks++;
            }
        }
        cout << "Total Tasks: " << totalTasks << endl;
        cout << "Completed Tasks: " << completedTasks << endl;
    }

    void dueDateAlerts() {
    cout << "Due Date Alerts:" << endl;
    time_t currentTime = time(nullptr);  // Use time function to get current time
    for (const Task& task : tasks) {
        if (task.dueDate.empty()) {
            continue;  // Skip tasks with no due date
        }
        // Convert dueDate to a time_t object for comparison
        struct tm dueDateStruct = {};
        stringstream dueDateStream(task.dueDate);
        dueDateStream >> get_time(&dueDateStruct, "%Y-%m-%d");  // Assuming due date format is YYYY-MM-DD

        if (mktime(&dueDateStruct) < currentTime) {
            cout << "Task \"" << task.name << "\" is overdue! Due Date: " << task.dueDate << endl;
        }
    }
}


    void addTaskNotes(int taskIndex, const string& notes) {
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].notes += notes;
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }
};


int main() {
    system("color c1");
    ToDoList todoList;
    cout<<endl;
    cout<<endl;
    cout<<endl;
    cout<<endl;
    cout<<endl;
    cout << "                                           ********************************************" << endl;
    cout << "                                           *          Welcome to ToDoList             *" << endl;
    cout<<endl;
    cout<<endl;
    cout<<endl;
    cout << "                                           *          Press any key to continue...    *" << endl;
    cout << "                                           ********************************************" << endl;

    cin.get();

    system("cls");

  system("color 30");
    while (true) {
        cout << "\nTodo List:\n";
        todoList.displayTasks();
        cout << "\nMenu:\n";
        cout << "1. Add Task\n";
        cout << "2. Remove Task\n";
        cout << "3. Mark Complete\n";
        cout << "4. Set Priority\n";
        cout << "5. Set Due Date\n";
        cout << "6. Add Category\n";
        cout << "7. Set Reminder\n";
        cout << "8. Display Statistics\n";
        cout << "9. Display Category Count\n";
        cout << "10. Set Task Dependencies\n";
        cout << "11. Display Task Dependencies\n";
        cout << "12. Visualize Task Dependencies\n";
        cout << "13. Remove Dependencies\n";
        cout << "14. Update Dependencies\n";
        cout << "15. Task Search\n";
        cout << "16. Due Date Alerts\n";
        cout << "17. Add Task Notes\n";
        cout << "18. Undo\n";
        cout << "19. Redo\n";
        cout << "20. Exit\n";
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string taskName;
                cout << "Enter task name: ";
                cin.ignore();
                getline(cin, taskName);
                todoList.addTask(taskName);
                break;
            }
            case 2: {
                int taskIndex;
                cout << "Enter task index to remove: ";
                cin >> taskIndex;
                todoList.removeTask(taskIndex - 1);
                break;
            }
            case 3: {
                int taskIndex;
                cout << "Enter task index to mark complete: ";
                cin >> taskIndex;
                todoList.markComplete(taskIndex - 1);
                break;
            }
            case 4: {
                int taskIndex, priority;
                cout << "Enter task index to set priority: ";
                cin >> taskIndex;
                cout << "Enter priority (1 - 5): ";
                cin >> priority;
                todoList.setTaskPriority(taskIndex - 1, priority);
                break;
            }
            case 5: {
                int taskIndex;
                string dueDate;
                cout << "Enter task index to set due date: ";
                cin >> taskIndex;
                cout << "Enter due date: ";
                cin >> dueDate;
                todoList.setTaskDueDate(taskIndex - 1, dueDate);
                break;
            }
            case 6: {
                int taskIndex;
                string category;
                cout << "Enter task index to add category: ";
                cin >> taskIndex;
                cout << "Enter category: ";
                cin.ignore();
                getline(cin, category);
                todoList.addTaskCategory(taskIndex - 1, category);
                break;
            }
            case 7: {
                int taskIndex;
                string reminderDate;
                cout << "Enter task index for setting reminder: ";
                cin >> taskIndex;
                cout << "Enter reminder date: ";
                cin >> reminderDate;
                todoList.setReminder(taskIndex - 1, reminderDate);
                break;
            }
            case 8:
                todoList.displayStatistics();
                break;
            case 9:
                todoList.displayCategoryCount();
                break;
            case 10: {
                int taskIndex;
                cout << "Enter task index to set dependencies: ";
                cin >> taskIndex;
                todoList.setTaskDependencies(taskIndex - 1);
                break;
            }
            case 11: {
                int taskIndex;
                cout << "Enter task index to display dependencies: ";
                cin >> taskIndex;
                todoList.displayTaskDependencies(taskIndex - 1);
                break;
            }
            case 12:
                todoList.visualizeTaskDependencies();
                break;
            case 13: {
                int dependentIndex, dependencyIndex;
                cout << "Enter dependent task index: ";
                cin >> dependentIndex;
                cout << "Enter dependency task index: ";
                cin >> dependencyIndex;
                todoList.removeDependency(dependentIndex - 1, dependencyIndex - 1);
                break;
            }
            case 14: {
                int taskIndex;
                cout << "Enter task index to update dependencies: ";
                cin >> taskIndex;
                todoList.updateDependencies(taskIndex - 1);
                break;
            }
            case 15: {
                string keyword;
                cout << "Enter keyword to search: ";
                cin >> keyword;
                todoList.taskSearch(keyword);
                break;
            }
            case 16:
                todoList.dueDateAlerts();
                break;
            case 17: {
                int taskIndex;
                string notes;
                cout << "Enter task index to add notes: ";
                cin >> taskIndex;
                cout << "Enter notes: ";
                cin.ignore();
                getline(cin, notes);
                todoList.addTaskNotes(taskIndex - 1, notes);
                break;
            }
                case 18: {
                    // Undo
                    todoList.undo();
                    break;
                }
                case 19: {
                    // Redo
                    todoList.redo();
                    break;
                }
            case 20:
                cout << "Exiting..." << endl;
                return 0;
            default:
                cout << "Invalid choice. Please select again." << endl;
        }
    }

    return 0;
}

