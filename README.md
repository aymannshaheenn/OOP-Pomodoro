# Pomodoro Task Manager (C++ Console App)

A fully-featured **Pomodoro Timer and Task Manager** built with **Object-Oriented Programming in C++**. This console-based productivity tool helps you manage tasks, track goals, earn achievements, and stay motivated with a colorful, interactive terminal interface.

---

## ✨ Features

### ✅ User System
- Sign up & login with password hashing
- User-specific saved durations and daily goals

### 🧠 Task Management
- Add, edit, delete tasks with priority & deadlines
- Tasks have a session count to track focus efforts
- Auto-suggests the next task based on urgency

### ⏱️ Timer System
- Traditional and Custom Pomodoro sessions
- Pause, resume, and complete sessions early
- Colorful progress bar and motivational quotes
- Automatic session summary with duration log

### 🗂️ History and Schedule
- View history of completed sessions
- View sorted schedule by priority and deadline
- Daily goal tracking and reminders

### 🏆 Goals & Achievements
- Add personal goals and mark them as achieved
- XP-based ranking system (Beginner → Master)
- Daily session targets with progress tracking

---

## 🎨 Terminal Colors & Style

- **Pink**: Motivational quotes
- **Green**: Headings and successful actions
- **Red/Orange/Yellow**: Task priorities (High/Med/Low)
- **Blue**: Session summaries
- **Cyan**: Session counters and task names

---

## 📁 File Structure

```
├── main.cpp                  # Source code
├── users.txt                # Stores usernames and hashed passwords
├── tasks.txt                # Task details
├── goals.txt                # Personal goals (active/achieved)
├── history.txt              # Session logs
├── xp.txt                   # XP and achievement tracking
├── durations_<username>.txt # Default durations per user
├── dailygoal_<username>.txt # Daily goal tracking per user
├── completed_sessions.txt   # Completed session log
```

---

## 🧪 How to Run

1. **Clone the Repository**

   ```bash
   git clone 
   cd pomodoro-task-manager
   ```

2. **Compile**

   Use any modern C++ compiler with C++11 support or higher.

   ```bash
   g++ -std=c++11 -o pomodoro main.cpp
   ```

3. **Run**

   ```bash
   ./pomodoro   # or pomodoro.exe on Windows
   ```

---

## 🔧 Requirements

- A C++11-compatible compiler (e.g., g++, MSVC)
- Windows Terminal or a terminal supporting ANSI escape codes for color

---

## 📌 Notes

- All data is saved in plain text files.
- Recommended for single-user mode on a single device.
- Ideal for learning OOP and file handling in C++.

---



