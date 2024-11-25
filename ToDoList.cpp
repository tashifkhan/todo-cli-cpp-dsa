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
#include <thread>
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
    stack<pair<string, Task> > undoStack;
    stack<pair<string, Task> > redoStack;
    priority_queue<pair<int, int> > priorityQueue;
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

    bool checkEmpty(){
        if (tasks.empty()){
            cout << "No tasks in the list. Please add a task first." << endl;
            return true;
        }
        return false;
    }

    void addTask(const string& taskName) {
        Task newTask(taskName);
        tasks.push_back(newTask);
        undoStack.push(make_pair("add", newTask));
        redoStack = stack<pair<string, Task> >();
        // Update priority queue and category count
        priorityQueue.push(make_pair(0, tasks.size() - 1));
        updateCategoryCount(newTask);
    }

    void removeTask(int taskIndex) {
        if (checkEmpty()) return;
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            Task removedTask = tasks[taskIndex];
            tasks.erase(tasks.begin() + taskIndex);
            undoStack.push(make_pair("remove", removedTask));
            redoStack = stack<pair<string, Task> >();

            // Update category count after removal
            updateCategoryCount(removedTask, true);
        }
    }

    void markComplete(int taskIndex) {
        if (checkEmpty()) return;
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].status = "Complete";
            undoStack.push(make_pair("mark_incomplete", tasks[taskIndex]));
            redoStack = stack<pair<string, Task> >();
        }
    }

    void setTaskPriority(int taskIndex, int priority) {
        if (checkEmpty()) return;
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            int oldPriority = tasks[taskIndex].priority;
            tasks[taskIndex].priority = priority;

            // Update priority queue if priority is changed
            if (oldPriority != priority) {
                priorityQueue.push(make_pair(priority, taskIndex));
            }
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }

    void setTaskDueDate(int taskIndex, const string& dueDate) {
        if (checkEmpty()) return;
        if (taskIndex < 0 || taskIndex >= tasks.size()) {
            cout << "Invalid task index. Please provide a valid task index." << endl;
            return;
        }

        // Check date format (YYYY-MM-DD)
        if (dueDate.length() != 10 || dueDate[4] != '-' || dueDate[7] != '-') {
            cout << "Invalid date format. Please use YYYY-MM-DD format." << endl;
            return;
        }

        try {
            int year = stoi(dueDate.substr(0, 4));
            int month = stoi(dueDate.substr(5, 2));
            int day = stoi(dueDate.substr(8, 2));

            // Basic date validation
            if (year < 2024 || month < 1 || month > 12 || day < 1 || day > 31) {
                cout << "Invalid date values." << endl;
                return;
            }

            // Check days in month
            bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            int daysInMonth[] = {31, (isLeapYear ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (day > daysInMonth[month - 1]) {
                cout << "Invalid day for the given month." << endl;
                return;
            }

            tasks[taskIndex].dueDate = dueDate;
            cout << "Due date set successfully." << endl;
        }
        catch (const exception& e) {
            cout << "Invalid date format. Please use YYYY-MM-DD format." << endl;
        }
    }

    void addTaskCategory(int taskIndex, const string& category) {
        if (checkEmpty()) return;
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].categories.push_back(category);
            updateCategoryCount(tasks[taskIndex]);
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }

    void setReminder(int taskIndex, const string& reminderDate) {
        if (checkEmpty()) return;
        if (taskIndex < 0 || taskIndex >= tasks.size()) {
            cout << "Invalid task index. Please provide a valid task index." << endl;
            return;
        }

        // Check date format (YYYY-MM-DD)
        if (reminderDate.length() != 10 || reminderDate[4] != '-' || reminderDate[7] != '-') {
            cout << "Invalid date format. Please use YYYY-MM-DD format." << endl;
            return;
        }

        try {
            int year = stoi(reminderDate.substr(0, 4));
            int month = stoi(reminderDate.substr(5, 2));
            int day = stoi(reminderDate.substr(8, 2));

            // Basic date validation
            if (year < 2024 || month < 1 || month > 12 || day < 1 || day > 31) {
                cout << "Invalid date values." << endl;
                return;
            }

            // Check days in month
            bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            int daysInMonth[] = {31, (isLeapYear ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (day > daysInMonth[month - 1]) {
                cout << "Invalid day for the given month." << endl;
                return;
            }

            tasks[taskIndex].reminderDate = reminderDate;
            cout << "Reminder set for task \"" << tasks[taskIndex].name << "\" on " << reminderDate << endl;
        }
        catch (const exception& e) {
            cout << "Invalid date format. Please use YYYY-MM-DD format." << endl;
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
            pair<string, Task> action = undoStack.top();
            undoStack.pop();

            if (action.first == "add") {
                tasks.pop_back();
                priorityQueue.pop();  // Remove from priority queue
                updateCategoryCount(action.second, true);
            } else if (action.first == "remove") {
                tasks.push_back(action.second);
                priorityQueue.push(make_pair(tasks.back().priority, tasks.size() - 1));
                updateCategoryCount(action.second);
            } else if (action.first == "mark_incomplete") {
                action.second.status = "Incomplete";
            }

            redoStack.push(action);
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            pair<string, Task> action = redoStack.top();
            redoStack.pop();

            if (action.first == "add") {
                tasks.push_back(action.second);
                priorityQueue.push(make_pair(tasks.back().priority, tasks.size() - 1));
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
        string input;
        while (true) {
            if (!(cin >> dependencyIndex)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a number or -1 to stop." << endl;
                continue;
            }
            
            if (dependencyIndex == -1) {
                break;
            }
            
            // Adjust for 1-based indexing and validate
            dependencyIndex--;
            if (dependencyIndex == taskIndex) {
                cout << "Task cannot depend on itself." << endl;
                continue;
            }
            
            if (dependencyIndex >= 0 && dependencyIndex < tasks.size()) {
                addDependency(taskIndex, dependencyIndex);
            } else {
                cout << "Invalid task index. Please provide a valid task index or -1 to stop." << endl;
            }
            
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

    void visualizeDependencyGraph() {
        cout << "\nDependency Graph:\n";
        for (size_t i = 0; i < tasks.size(); ++i) {
            // Print task name
            cout << "\n" << tasks[i].name << "\n";
            
            // Print arrow connections
            for (int depIndex : tasks[i].dependencies) {
                cout << "|\n";
                cout << "└─> " << tasks[depIndex].name << "\n";
            }
            
            // Print separator
            if (!tasks[i].dependencies.empty()) {
                cout << "\n";
            }
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

            visualizeDependencyGraph();
        }
    }

    void removeDependency(int dependentIndex, int dependencyIndex) {
        if (checkEmpty()) return;
        if (dependentIndex >= 0 && dependentIndex < tasks.size() && dependencyIndex >= 0 && dependencyIndex < tasks.size()) {
            tasks[dependentIndex].dependencies.erase(dependencyIndex);
        } else {
            cout << "Invalid task indices. Please provide valid task indices." << endl;
        }
    }

    void updateDependencies(int taskIndex) {
        if (checkEmpty()) return;
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
    
    // void searchWithBoyerMoore(const string& text, const string& pattern) {
    //     int n = text.length();
    //     int m = pattern.length();
    //     vector<int> badChar(256, -1);

    //     // Preprocessing for bad character heuristic
    //     for (int i = 0; i < m; i++) {
    //         badChar[static_cast<unsigned char>(pattern[i])] = i;
    //     }

    //     cout << "Search results for pattern \"" << pattern << "\":" << endl;
        
    //     int s = 0;
    //     bool found = false;
    //     while (s <= (n - m)) {
    //         int j = m - 1;

    //         while (j >= 0 && pattern[j] == text[s + j]) {
    //             j--;
    //         }

    //         if (j < 0) {
    //             cout << "Pattern found at index " << s << endl;
    //             found = true;
    //             s += (s + m < n) ? m - badChar[static_cast<unsigned char>(text[s + m])] : 1;
    //         } else {
    //             s += max(1, j - badChar[static_cast<unsigned char>(text[s + j])]);
    //         }
    //     }

    //     if (!found) {
    //         cout << "Pattern not found in the text." << endl;
    //     }
    // }

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

        if (tasks.empty()) {
            cout << "No tasks in the list. No due date alerts to show." << endl;
            return;
        }

        bool hasDueDates = false;
        for (const Task& task : tasks) {
            if (!task.dueDate.empty()) {
                hasDueDates = true;
                break;
            }
        }

        if (!hasDueDates) {
            cout << "No tasks have due dates set. You can relax for now! 😊" << endl;
            return;
        }

        cout << "Due Date Alerts:" << endl;
        time_t currentTime = time(nullptr);

        // Create vector of pairs (dueDate, taskName) for sorting
        vector<pair<time_t, string>> dueDates;
        
        for (const Task& task : tasks) {
            if (!task.dueDate.empty()) {
                struct tm dueDateStruct = {};
                stringstream dueDateStream(task.dueDate);
                dueDateStream >> get_time(&dueDateStruct, "%Y-%m-%d");
                time_t dueTime = mktime(&dueDateStruct);
                dueDates.push_back({dueTime, task.name + " (Due: " + task.dueDate + ")"});
            }
        }

        // Sort by due date
        sort(dueDates.begin(), dueDates.end());

        // Display sorted tasks
        for (const auto& task : dueDates) {
            if (task.first < currentTime) {
                cout << "⚠️  OVERDUE: " << task.second << endl;
            } else {
                cout << "📅 Upcoming: " << task.second << endl;
            }
        }
    }


    void addTaskNotes(int taskIndex, const string& notes) {
        if (checkEmpty()) return;
        if (taskIndex >= 0 && taskIndex < tasks.size()) {
            tasks[taskIndex].notes += notes;
        } else {
            cout << "Invalid task index. Please provide a valid task index." << endl;
        }
    }

    size_t getTaskCount() const {
        return tasks.size();
    }

    void changeCategory(const string& category, int count) {
        categoryCount[category] = count;
    }

void changeTaskCategory(int taskIndex) {
    if (checkEmpty()) return;
    if (taskIndex >= 0 && taskIndex < tasks.size()) {
        // Display current categories for the task
        cout << "Current categories for task \"" << tasks[taskIndex].name << "\":" << endl;
        if (tasks[taskIndex].categories.empty()) {
            cout << "No categories assigned." << endl;
            return;
        }
        
        for (size_t i = 0; i < tasks[taskIndex].categories.size(); i++) {
            cout << i + 1 << ". " << tasks[taskIndex].categories[i] << endl;
        }

        // Ask which category to modify
        cout << "Enter the index of category to modify (1-" << tasks[taskIndex].categories.size() << "): ";
        int categoryIndex;
        cin >> categoryIndex;
        categoryIndex--; // Convert to 0-based index

        if (categoryIndex >= 0 && categoryIndex < tasks[taskIndex].categories.size()) {
            string oldCategory = tasks[taskIndex].categories[categoryIndex];
            cout << "Enter new category name: ";
            cin.ignore();
            string newCategory;
            getline(cin, newCategory);

            // Update category counts
            categoryCount[oldCategory]--;
            categoryCount[newCategory]++;

            // Update category name
            tasks[taskIndex].categories[categoryIndex] = newCategory;
            cout << "Category updated successfully!" << endl;
        } else {
            cout << "Invalid category index." << endl;
        }
    } else {
        cout << "Invalid task index." << endl;
    }
}
};


int main() {
    ToDoList todoList;
    cout << "\n\n";
    cout << "   ╔══════════════════════════════════════════════════╗" << endl;
    cout << "   ║                 Welcome to ToDoList              ║" << endl;
    cout << "   ║                                                  ║" << endl;
    cout << "   ║         Organize. Prioritize. Accomplish.        ║" << endl;
    cout << "   ║                                                  ║" << endl;
    cout << "   ║            Press any key to continue...          ║" << endl;
    cout << "   ╚══════════════════════════════════════════════════╝" << endl;
    cout << "\n\n";

    cin.get();

    // system("clear");
    int run_counter = 0;
    while (true) {
        if (run_counter != 0){
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        system("clear");
        if (todoList.getTaskCount() > 0) {
            cout << "\n📋 Your Todo List:\n";
        }
        else{
            if(run_counter>0)
                cout << "\n📋 Your Todo List is empty.\n";
        }
        todoList.displayTasks();
        cout << "\n🎯  Menu Options:\n";
        cout << "1.  ➕ Add New Task\n";
        cout << "2.  ❌ Remove Task\n";
        cout << "3.  ✅ Mark Complete\n";
        cout << "4.  ⭐️ Set Priority\n";
        cout << "5.  📅 Set Due Date\n";
        cout << "6.  🏷️ Add Category\n";
        cout << "7.  ⏰ Set Reminder\n";
        cout << "8.  📊 Display Statistics\n";
        cout << "9.  📈 Display Category Count\n";
        cout << "10. 🔗 Set Task Dependencies\n";
        cout << "11. 👀 Display Task Dependencies\n";
        cout << "12. 🌳 Visualize Task Dependencies\n";
        cout << "13. 💔 Remove Dependencies\n";
        cout << "14. 🔄 Update Dependencies\n";
        cout << "15. 🔍 Task Search\n";
        cout << "16. ⚠️  Due Date Alerts\n";
        cout << "17. 📝 Add Task Notes\n";
        cout << "18. ↩️  Undo\n";
        cout << "19. ↪️  Redo\n";
        cout << "20. 🚪 Exit\n";

        int choice;
        cout << "\n👉 Enter your choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                string taskName;
                cout << "📝 Enter task name: ";
                cin.ignore();
                getline(cin, taskName);
                todoList.addTask(taskName);

                // Additional task details
                cout << "\n✨ Would you like to add more details to this task? (y/n): ";
                char more;
                cin >> more;
                if (more == 'y' || more == 'Y') {
                    string input;
                    
                    // Priority
                    cout << "⭐️ Enter priority (1-5, or press Enter to skip): ";
                    cin.ignore();
                    getline(cin, input);
                    if (!input.empty()) {
                        todoList.setTaskPriority(todoList.getTaskCount() - 1, stoi(input));
                    }

                    // Due date
                    cout << "📅 Enter due date (YYYY-MM-DD, or press Enter to skip): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        todoList.setTaskDueDate(todoList.getTaskCount() - 1, input);
                    }

                    // Category
                    cout << "🏷️  Enter category (or press Enter to skip): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        todoList.addTaskCategory(todoList.getTaskCount() - 1, input);
                    }

                    // Notes
                    cout << "📝 Enter any notes (or press Enter to skip): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        todoList.addTaskNotes(todoList.getTaskCount() - 1, input);
                    }

                    // Reminder
                    cout << "⏰ Enter reminder date (YYYY-MM-DD, or press Enter to skip): ";
                    getline(cin, input);
                    if (!input.empty()) {
                        todoList.setReminder(todoList.getTaskCount() - 1, input);
                    }

                    cout << "✨ Task details added successfully! ✨\n";
                }
                break;
            }


            case 2: {
                if (todoList.checkEmpty()) break;
                int taskIndex;
                cout << "Enter task index to remove: ";
                cin >> taskIndex;
                todoList.removeTask(taskIndex - 1);
                break;
            }

            case 3: {
                if (todoList.checkEmpty()) break;
                int taskIndex;
                cout << "Enter task index to mark complete: ";
                cin >> taskIndex;
                todoList.markComplete(taskIndex - 1);
                break;
            }

            case 4: {
                if (todoList.checkEmpty()) break;
                int taskIndex, priority;
                cout << "Enter task index to set priority: ";
                cin >> taskIndex;
                do {
                    cout << "Enter priority (1 - 5): ";
                    cin >> priority;
                    if (priority < 1 || priority > 5) {
                        cout << "Invalid priority. Please enter a number between 1 and 5.\n";
                    }
                } while (priority < 1 || priority > 5);
                todoList.setTaskPriority(taskIndex - 1, priority);
                break;
            }

            case 5: {
                if (todoList.checkEmpty()) break;
                int taskIndex;
                string dueDate;
                cout << "Enter task index to set due date: ";
                cin >> taskIndex;
                cout << "Enter due date (YYYY-MM-DD): ";
                cin >> dueDate;

                // Validate date format and values
                if (dueDate.length() != 10 || dueDate[4] != '-' || dueDate[7] != '-') {
                    cout << "Invalid date format. Please use YYYY-MM-DD\n";
                    break;
                }

                try {
                    int year = stoi(dueDate.substr(0, 4));
                    int month = stoi(dueDate.substr(5, 2));
                    int day = stoi(dueDate.substr(8, 2));

                    if (year < 2024 || month < 1 || month > 12 || day < 1 || day > 31) {
                        cout << "Invalid date values.\n";
                        break;
                    }

                    // Additional validation for days in month
                    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
                    int daysInMonth[] = {31, (isLeapYear ? 29 : 28), 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                    if (day > daysInMonth[month - 1]) {
                        cout << "Invalid day for the given month.\n";
                        break;
                    }

                    todoList.setTaskDueDate(taskIndex - 1, dueDate);
                } catch (const exception& e) {
                    cout << "Invalid date format. Please use YYYY-MM-DD\n";
                }
                break;
            }

            case 6: {
                if (todoList.checkEmpty()) break;
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
                if (todoList.checkEmpty()) break;
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
                if (todoList.checkEmpty()) break;
                int taskIndex;
                cout << "Enter task index to set dependencies: ";
                cin >> taskIndex;
                todoList.setTaskDependencies(taskIndex - 1);
                break;
            }

            case 11: {
                if (todoList.checkEmpty()) break;
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
                if (todoList.checkEmpty()) break;
                int dependentIndex, dependencyIndex;
                cout << "Enter dependent task index: ";
                cin >> dependentIndex;
                cout << "Enter dependency task index: ";
                cin >> dependencyIndex;
                todoList.removeDependency(dependentIndex - 1, dependencyIndex - 1);
                break;
            }

            case 14: {
                if (todoList.checkEmpty()) break;
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
                if (todoList.checkEmpty()) break;
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
                cout << "👋 Thank you for using ToDo List! Goodbye!\n";
                return 0;
            
            default:
                cout << "❌ Invalid choice. Please try again.\n";

        }
        run_counter++;
    }

    return 0;
}

void displayProgressBar(int completed, int total) {
    const int barWidth = 50;
    float progress = (float)completed / total;
    int pos = barWidth * progress;
    
    cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) cout << "=";
        else if (i == pos) cout << ">";
        else cout << " ";
    }
    cout << "] " << int(progress * 100.0) << "%\n";
}