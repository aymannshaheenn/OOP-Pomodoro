#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <thread>
#include <atomic>
#include <conio.h>
#include <algorithm>

using namespace std;
atomic<bool> isTimerFinished(false);

atomic<bool> isPaused(false);
atomic<bool> isStopped(false);
int secondsElapsed = 0;

void countdownProgressBar(int minutes) {
    vector<string> quotes = {
        "Push through the session—you've got this.",
        "Stay focused, one minute at a time.",
        "Discipline beats motivation.",
        "Little progress is still progress.",
        "Consistency builds greatness.",
        "You don’t have to be extreme, just consistent.",
        "Big results require small steps repeated daily.",
        "Every focused minute counts.",
        "The only bad session is the one that didn’t happen.",
        "You’re closer than you think—stay with it."
    };

    srand(static_cast<unsigned int>(time(0)));

    string quote = quotes[rand() % quotes.size()];
    cout << "\033[38;5;198m" << "\nMotivation: " << quote << "\033[0m\n\n";

    int totalSeconds = minutes * 60;
    for (int elapsed = 0; elapsed <= totalSeconds; ++elapsed) {
        secondsElapsed = elapsed;

        if (isStopped) {
            cout << "\nSession stopped early.\n";
            return;
        }

        while (isPaused) {
            this_thread::sleep_for(chrono::milliseconds(500));
        }

        int remaining = totalSeconds - elapsed;
        int min = remaining / 60;
        int sec = remaining % 60;

        float percent = (float)elapsed / totalSeconds;
        int barWidth = 30;
        int pos = static_cast<int>(barWidth * percent);


        cout << "\r[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) cout << "\033[42m \033[0m";
            else cout << " ";
        }
        cout << "] ";
        cout << setw(2) << setfill('0') << min << ":" << setw(2) << setfill('0') << sec;
        cout << flush;
        this_thread::sleep_for(chrono::seconds(1));
    }
    cout << "\n\033[33mTime's up!\033[0m\n";
    for (int i = 0; i < 3; ++i) {
        cout << "\a";
        this_thread::sleep_for(chrono::milliseconds(500));
    }
    isTimerFinished = true;


}

string getCurrentTime() {
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &ltm);
    return string(buffer);
}
string hashPassword(const string& password) {
    hash<string> hasher;
    return to_string(hasher(password));
}

class UserManager {
public:
    string hashPassword(const string& password) {
        hash<string> hasher;
        return to_string(hasher(password));
    }
    bool signUp(string username, string password) {
        ofstream out("users.txt", ios::app);
        if (!out) return false;
        out << username << " " << hashPassword(password) << endl;
        return true;
    }
    void saveDurations(const string& username, int work, int brk) {
        ofstream out("durations_" + username + ".txt");
        out << work << " " << brk;
    }

    pair<int, int> loadDurations(const string& username) {
        ifstream in("durations_" + username + ".txt");
        int work = 25, brk = 5;
        if (in >> work >> brk) return { work, brk };
        return { 25, 5 }; // Default fallback
    }

    bool login(string username, string password) {
        ifstream in("users.txt");
        string u, p;
        string hashedInput = hashPassword(password);
        while (in >> u >> p) {
            if (u == username && p == hashedInput)
                return true;
        }
        return false;
    }

};

class Task {
public:
    string title;
    string priority;
    string deadline;
    int sessions;
    Task() : sessions(0) {}

    Task(string t, string p, string d, int s)
        : title(t), priority(p), deadline(d), sessions(s) {
    }
};
string colorPriority(const string& priority) {
    if (priority == "HIGH") return "\033[31mHIGH\033[0m";           // Red
    else if (priority == "MED") return "\033[38;5;208mMED\033[0m";  // Orange
    else if (priority == "LOW") return "\033[33mLOW\033[0m";        // Yellow
    else return priority;
}

class TaskManager {
    vector<Task> tasks;

public:
    void addTask(Task t) {
        tasks.push_back(t);
        ofstream out("tasks.txt", ios::app);
        out << t.title << "|" << t.priority << "|" << t.deadline << "|" << t.sessions << endl;
    }
    void decrementTaskSession(const string& taskName) {
        for (auto& t : tasks) {
            if (t.title == taskName && t.sessions > 0) {
                t.sessions--;
                break;
            }
        }

        // Rewrite updated tasks back to file
        ofstream out("tasks.txt");
        for (const auto& t : tasks) {
            out << t.title << "|" << t.priority << "|" << t.deadline << "|" << t.sessions << endl;
        }
    }

    void editOrDeleteTasks() {
        if (tasks.empty()) {
            cout << "\nNo tasks available.\n";
            return;
        }

        cout << "\nYour Tasks:\n";
        for (size_t i = 0; i < tasks.size(); ++i) {
            cout << i + 1 << ". " << tasks[i].title
                << " (" << colorPriority(tasks[i].priority) << "), Due: "
                << tasks[i].deadline << ", Sessions: " << tasks[i].sessions << endl;
        }

        int choice;
        cout << "\n1. Edit Task\n2. Delete Task\nChoose: ";
        cin >> choice;

        int index;
        cout << "Enter task number: ";
        cin >> index;
        index--;

        if (index < 0 || index >= static_cast<int>(tasks.size())) {
            cout << "Invalid task number.\n";
            return;
        }

        if (choice == 1) {
            string title, priority, deadline;
            int sessions;
            cout << "New title: "; cin >> title;
            cout << "New priority (HIGH/MED/LOW): "; cin >> priority;
            cout << "New deadline (YYYY-MM-DD): "; cin >> deadline;
            cout << "New session count: "; cin >> sessions;

            tasks[index] = Task(title, priority, deadline, sessions);
            cout << "Task updated!\n";
        }
        else if (choice == 2) {
            tasks.erase(tasks.begin() + index);
            cout << "Task deleted!\n";
        }

        // Overwrite the tasks file
        ofstream out("tasks.txt");
        for (const auto& t : tasks) {
            out << t.title << "|" << t.priority << "|" << t.deadline << "|" << t.sessions << endl;
        }
    }

    void filterTasks() {
        int option;
        cout << "\nFilter Tasks:\n1. By Priority\n2. By Deadline\nChoose: ";
        cin >> option;

        if (option == 1) {
            string prio;
            cout << "Enter priority (HIGH/MED/LOW): ";
            cin >> prio;

            cout << "\nTasks with Priority " << prio << ":\n";
            for (const auto& t : tasks) {
                if (t.priority == prio)
                    cout << t.title << " (" << colorPriority(t.priority) << "), Due: " << t.deadline << ", Sessions: " << t.sessions << endl;
            }
        }
        else if (option == 2) {
            string date;
            cout << "Enter deadline (YYYY-MM-DD): ";
            cin >> date;

            cout << "\nTasks Due Before " << date << ":\n";
            for (const auto& t : tasks) {
                if (t.deadline < date)
                    cout << t.title << " (" << colorPriority(t.priority) << "), Due: " << t.deadline << ", Sessions: " << t.sessions << endl;
            }
        }
        else {
            cout << "Invalid choice.\n";
        }
    }

    void showTasks() {
        cout << "\nYour Tasks:\n";
        for (auto& t : tasks) {
            cout << t.title << " (" << t.priority << "), Due: " << t.deadline
                << ", \033[36mRemaining Sessions: " << t.sessions << "\033[0m\n";


        }
    }

    void loadTasks() {
        ifstream in("tasks.txt");
        string line;
        while (getline(in, line)) {
            size_t p1 = line.find('|');
            size_t p2 = line.find('|', p1 + 1);
            size_t p3 = line.find('|', p2 + 1);
            string title = line.substr(0, p1);
            string priority = line.substr(p1 + 1, p2 - p1 - 1);
            string deadline = line.substr(p2 + 1, p3 - p2 - 1);
            int sessions = stoi(line.substr(p3 + 1));
            tasks.push_back(Task(title, priority, deadline, sessions));
        }
    }

    vector<Task>& getTasks() {
        return tasks;
    }

};



class Timer {
public:
    virtual void startSession(string task, int workDuration, int breakDuration) = 0;

    void handleSession(int duration) {
        isPaused = false;
        isStopped = false;
        secondsElapsed = 0;

        cout << "[Controls] Press 'p' = Pause | 'r' = Resume | 'c' = Complete Early\n";
        // Calculate estimated end time
        time_t now = time(0);
        time_t endTime = now + duration * 60;
        tm localEnd;
        localtime_s(&localEnd, &endTime);
        char buf[10];
        strftime(buf, sizeof(buf), "%I:%M %p", &localEnd);
        cout << "Session will end at: \033[32m" << buf << "\033[0m\n";

        thread countdownThread(countdownProgressBar, duration);

        while (!isStopped && countdownThread.joinable()) {
            if (isTimerFinished) break;

            if (_kbhit()) {
                char cmd = tolower(_getch());
                if (cmd == 'p') {
                    isPaused = true;
                    cout << "\n\033[33m Paused...\033[0m\n";
                }
                else if (cmd == 'r') {
                    isPaused = false;
                    cout << "\n\033[32m Resumed.\033[0m\n";
                }
                else if (cmd == 'c') {
                    isStopped = true;
                }
            }

            this_thread::sleep_for(chrono::milliseconds(100));
        }

        if (countdownThread.joinable()) countdownThread.join();
        isTimerFinished = false;

    }
};

class TraditionalTimer : public Timer {
public:
    void startSession(string task, int workDuration, int breakDuration) override {
        cout << "\nTraditional Pomodoro for: " << task << "\nWork:\n";
        handleSession(workDuration);
        if (!isStopped) {
            cout << "Break:\n";
            handleSession(breakDuration);
        }
    }
};

class CustomTimer : public Timer {
public:
    void startSession(string task, int workDuration, int breakDuration) override {
        cout << "\nCustom Pomodoro for: " << task << "\nWork:\n";
        handleSession(workDuration);
        if (!isStopped) {
            cout << "Break:\n";
            handleSession(breakDuration);
        }
    }
};

class HistoryManager {
public:
    void log(string type, string task, int duration) {
        ofstream out("history.txt", ios::app);
        out << type << "|" << task << "|" << secondsElapsed << "|" << getCurrentTime() << endl;
    }
    void logCompletedSession(const string& task, int minutes) {
        ofstream out("completed_sessions.txt", ios::app);
        out << task << " | " << minutes << " minutes | " << getCurrentTime() << endl;
    }

    void showHistory() {
        ifstream in("history.txt");
        string type, task, time;
        int duration;
        cout << "\nHistory:\n";
        while (getline(in, type, '|') && getline(in, task, '|') && in >> duration && in.ignore() && getline(in, time)) {
            cout << "[" << type << "] Task: " << task << " | Time Elapsed: " << duration << " sec | " << time << endl;
        }
    }
};

class Goal {
public:
    string text;
    bool achieved;
    Goal(string t) : text(t), achieved(false) {}
};

class GoalManager {
    vector<Goal> goals;

public:
    void addGoal(string goal) {
        goals.emplace_back(goal);
        ofstream out("goals.txt", ios::app);
        out << goal << "|0\n";
    }

    void completeGoal(int index) {
        if (index < 0 || index >= goals.size()) return;
        goals[index].achieved = true;
        saveGoals();
    }

    void showGoals() {
        cout << "\nGoals:\n";
        for (int i = 0; i < goals.size(); ++i) {
            cout << i + 1 << ". " << goals[i].text << (goals[i].achieved ? " (Achieved)" : " (Active)") << endl;
        }
    }

    void loadGoals() {
        ifstream in("goals.txt");
        string line;
        while (getline(in, line)) {
            size_t sep = line.find('|');
            string text = line.substr(0, sep);
            bool achieved = (line[sep + 1] == '1');
            Goal g(text);
            g.achieved = achieved;
            goals.push_back(g);
        }
    }

    void saveGoals() {
        ofstream out("goals.txt");
        for (auto& g : goals) {
            out << g.text << "|" << (g.achieved ? "1" : "0") << "\n";
        }
    }
};

class AchievementManager {
    int xp;
    string rank;

public:
    AchievementManager() : xp(0), rank("Beginner") {
        ifstream in("xp.txt");
        in >> xp;
        updateRank();
    }

    void addXP(int amount) {
        xp += amount;
        ofstream out("xp.txt");
        out << xp;
        updateRank();
    }

    void updateRank() {
        if (xp >= 100) rank = "Master";
        else if (xp >= 50) rank = "Advanced";
        else if (xp >= 20) rank = "Intermediate";
        else rank = "Beginner";
    }

    void showAchievements() {
        cout << "\nAchievements: XP = " << xp << " | Rank = " << rank << endl;
    }
};
class DailyGoalManager {
    int target = 0;
    int completed = 0;
    string lastDate;
    string username;

    string getToday() {
        time_t now = time(0);
        tm ltm;
        localtime_s(&ltm, &now);
        char buffer[11];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &ltm);
        return string(buffer);
    }

public:
    DailyGoalManager(const string& user) : username(user) {
        load();
    }

    void load() {
        ifstream in("dailygoal_" + username + ".txt");
        string today = getToday();
        if (in >> lastDate >> target >> completed) {
            if (lastDate != today) {
                completed = 0;  // new day
                lastDate = today;
                save();
            }
        }
        else {
            lastDate = today;
            target = 0;
            completed = 0;
            save();
        }
    }

    void save() {
        ofstream out("dailygoal_" + username + ".txt");
        out << lastDate << " " << target << " " << completed;
    }

    void increment() {
        completed++;
        save();
    }

    void setTarget(int t) {
        target = t;
        save();
    }

    void showProgress() {
        cout << "\n\033[35mDaily Goal Progress:\033[0m " << completed << "/" << target;
        if (target > 0) {
            if (completed >= target)
                cout << " Done\n";
            else
                cout << " (" << (target - completed) << " to go)\n";
        }
        else {
            cout << " (No goal set)\n";
        }
    }
};

class ScheduleManager {
public:
    void showSchedule(const vector<Task>& tasks) {
        cout << "\nYour Schedule (Sorted by Deadline):\n";
        vector<Task> filtered;
        for (const auto& t : tasks) {
            if (t.sessions > 0)
                filtered.push_back(t);
        }
        auto priorityRank = [](const string& p) {
            if (p == "HIGH") return 1;
            if (p == "MED") return 2;
            return 3; // LOW or anything else
            };

        sort(filtered.begin(), filtered.end(), [&](const Task& a, const Task& b) {
            if (priorityRank(a.priority) != priorityRank(b.priority))
                return priorityRank(a.priority) < priorityRank(b.priority);
            return a.deadline < b.deadline;
            });


        for (const auto& t : filtered) {
            cout << "[" << setw(5) << left << t.priority << "]  "
                << setw(20) << left << t.title
                << "(Due: " << t.deadline << ", \033[36mSessions: " << t.sessions << "\033[0m)\n";


        }

        if (filtered.empty()) {
            cout << "No active tasks with sessions remaining.\n";
        }
    }
};

class PomodoroApp {
    void showSessionSummary(const string& task, int duration, bool stoppedEarly) {
        cout << "\n\033[1;34m====== Session Summary ======\033[0m\n";
        cout << "Task: \033[36m" << task << "\033[0m\n";
        cout << "Planned Work Duration: " << duration << " minutes\n";
        cout << "Status: " << (stoppedEarly ? "\033[31mStopped Early\033[0m" : "\033[32mCompleted\033[0m") << "\n";
        cout << "Actual Time Elapsed: " << secondsElapsed / 60 << "m " << secondsElapsed % 60 << "s\n";
        cout << "Timestamp: " << getCurrentTime() << "\n";
        cout << "\033[1;34m=============================\033[0m\n";
    }

    UserManager userManager;
    TaskManager taskManager;
    HistoryManager historyManager;
    GoalManager goalManager;
    AchievementManager achievementManager;
    ScheduleManager scheduleManager;
    Timer* timer;
    string currentUser;
    int defaultWorkDuration = 25;
    int defaultBreakDuration = 5;
    DailyGoalManager* dailyGoalManager = nullptr;



public:
    PomodoroApp() : timer(nullptr) {}
    void showUpcomingTasks() {
        cout << "\n\033[1;33mUpcoming Tasks Due Today or Tomorrow:\033[0m\n";

        time_t now = time(0);
        tm todayTm, tomorrowTm;
        localtime_s(&todayTm, &now);
        tomorrowTm = todayTm;
        tomorrowTm.tm_mday += 1;
        mktime(&tomorrowTm); // Normalize

        char bufToday[11], bufTomorrow[11];
        strftime(bufToday, sizeof(bufToday), "%Y-%m-%d", &todayTm);
        strftime(bufTomorrow, sizeof(bufTomorrow), "%Y-%m-%d", &tomorrowTm);

        bool found = false;
        for (const auto& t : taskManager.getTasks()) {
            if (t.deadline == bufToday || t.deadline == bufTomorrow) {
                found = true;
                cout << "- " << t.title << " (" << colorPriority(t.priority)
                    << "), Due: " << t.deadline << ", Sessions: " << t.sessions << "\n";
            }
        }

        if (!found)
            cout << "No tasks due today or tomorrow.\n";
    }
    string suggestEarliestTask() {
        const auto& tasks = taskManager.getTasks();
        string bestTask = "";
        string earliest = "9999-12-31"; // very far future

        for (const auto& t : tasks) {
            if (t.sessions > 0 && t.deadline < earliest) {
                earliest = t.deadline;
                bestTask = t.title;
            }
        }

        return bestTask;
    }

    void run() {
        int choice;
        cout << "1. Sign Up\n2. Login\nChoose: ";
        cin >> choice;

        string user, pass;
        cout << "Username: "; cin >> user;
        cout << "Password: "; cin >> pass;

        if (choice == 1) {
            userManager.signUp(user, pass);
            cout << "Signed up!\n";
        }
        else {
            if (!userManager.login(user, pass)) {
                cout << "Login failed.\n";
                return;
            }
            cout << "Logged in!\n";
            taskManager.loadTasks(); // already exists here
            showUpcomingTasks();     // <- add this

        }

        currentUser = user;

        tie(defaultWorkDuration, defaultBreakDuration) = userManager.loadDurations(user);
        dailyGoalManager = new DailyGoalManager(currentUser);

        taskManager.loadTasks();
        goalManager.loadGoals();

        while (true) {
            cout << "\n\033[1;32m================== Pomodoro Menu ==================\033[0m\n";
            cout << " 1. Add Task\n";
            cout << " 2. Start Traditional Timer\n";
            cout << " 3. Start Custom Timer\n";
            cout << " 4. View History\n";
            cout << " 5. Manage Goals\n";
            cout << " 6. View Achievements\n";
            cout << " 7. View Schedule\n";
            cout << " 8. Set Default Durations\n";
            cout << " 9. Show Daily Goal Progress\n";
            cout << "10. Set Daily Session Goal\n";
            cout << "11. Exit\n";
            cout << "\033[1;32m===================================================\033[0m\n";
            cout << "Choose: ";





            cin >> choice;

            if (choice == 1) {
                string t, p, d; int s;
                cout << "Title: "; cin >> t;
                cout << "Priority (HIGH/MED/LOW): "; cin >> p;
                cout << "Deadline (YYYY-MM-DD): "; cin >> d;
                cout << "Sessions: "; cin >> s;
                taskManager.addTask(Task(t, p, d, s));
            }
            else if (choice == 2 || choice == 3) {
                string task;
                cout << "Task name (press Enter to auto-suggest): ";
                cin.ignore(); // clear newline
                getline(cin, task);
                if (task.empty()) {
                    task = suggestEarliestTask();
                    if (task.empty()) {
                        cout << "No available tasks with sessions remaining.\n";
                        continue;
                    }
                    else {
                        cout << "Suggested Task: \033[36m" << task << "\033[0m\n";
                    }
                }

                int w = defaultWorkDuration, b = defaultBreakDuration;


                if (choice == 3) {
                    cout << "Work (mins): "; cin >> w;
                    cout << "Break (mins): "; cin >> b;
                    timer = new CustomTimer();
                }
                else {
                    timer = new TraditionalTimer();
                }

                timer->startSession(task, w, b);
                showSessionSummary(task, w, isStopped);

                historyManager.log((choice == 2 ? "Traditional" : "Custom"), task, w);
                if (!isStopped) {
                    historyManager.logCompletedSession(task, w);
                }

                achievementManager.addXP(10);
                taskManager.decrementTaskSession(task);
                dailyGoalManager->increment();



                int next;
                cout << "\nWhat would you like to do next?\n1. Continue\n2. Exit\nChoice: ";
                cin >> next;
                if (next == 2) break;
                delete timer;
                timer = nullptr;
            }
            else if (choice == 4) {
                historyManager.showHistory();
            }
            else if (choice == 5) {
                int gChoice;
                cout << "1. Add Goal\n2. Show Goals\n3. Mark Achieved\nChoose: ";
                cin >> gChoice;
                if (gChoice == 1) {
                    string goal;
                    cout << "Enter goal: "; cin.ignore(); getline(cin, goal);
                    goalManager.addGoal(goal);
                }
                else if (gChoice == 2) {
                    goalManager.showGoals();
                }
                else if (gChoice == 3) {
                    int idx;
                    goalManager.showGoals();
                    cout << "Goal number to complete: "; cin >> idx;
                    goalManager.completeGoal(idx - 1);
                    achievementManager.addXP(5);
                }
            }
            else if (choice == 6) {
                achievementManager.showAchievements();
            }
            else if (choice == 7) {
                scheduleManager.showSchedule(taskManager.getTasks());
            }
            else if (choice == 8) {
                taskManager.filterTasks();
            }
            else if (choice == 9) {
                taskManager.editOrDeleteTasks();
            }
            else if (choice == 10) {
                cout << "New default work duration (minutes): ";
                cin >> defaultWorkDuration;
                cout << "New default break duration (minutes): ";
                cin >> defaultBreakDuration;
                userManager.saveDurations(currentUser, defaultWorkDuration, defaultBreakDuration);
                cout << "Defaults updated!\n";
            }
            else if (choice == 11) {
                dailyGoalManager->showProgress();
            }
            else if (choice == 12) {
                int t;
                cout << "Enter number of sessions you want to complete today: ";
                cin >> t;
                dailyGoalManager->setTarget(t);
                cout << "Goal set!\n";
            }
            else if (choice == 13) {
                cout << "Goodbye!\n";
                break;
            }




            else {
                cout << "Goodbye!\n";
                break;
            }
        }
        if (timer) delete timer;
        if (dailyGoalManager) delete dailyGoalManager;

    }
};

int main() {
    PomodoroApp app;
    app.run();
    return 0;
}
