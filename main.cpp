#include <iostream>
#include <string>
#include <limits>
#include "TaskManager.h"
#include "Sorter.h"

using namespace std;

// ── Помощни функции за четене от терминала ──

int readInt(const string& prompt) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "[ГРЕШКА] Въведете число.\n";
    }
}

string readLine(const string& prompt) {
    string val;
    cout << prompt;
    getline(cin, val);
    return val;
}

Priority readPriority() {
    cout << "  Приоритет (1=LOW, 2=MEDIUM, 3=HIGH): ";
    int c = readInt("");
    if (c == 1) return Priority::LOW;
    if (c == 3) return Priority::HIGH;
    return Priority::MEDIUM;
}

Status readStatus() {
    cout << "  Статус (1=TODO, 2=IN_PROGRESS, 3=DONE): ";
    int c = readInt("");
    if (c == 2) return Status::IN_PROGRESS;
    if (c == 3) return Status::DONE;
    return Status::TODO;
}

// ── Менюта ──

void menuProekti(TaskManager& manager) {
    while (true) {
        cout << "\n--- ПРОЕКТИ ---\n"
             << "1. Създаване на проект\n"
             << "2. Редактиране на проект\n"
             << "3. Изтриване на проект\n"
             << "4. Покажи всички\n"
             << "0. Назад\n";
        int choice = readInt("Избор: ");
        if (choice == 0) break;

        if (choice == 1) {
            string name = readLine("  Име на проект: ");
            string desc = readLine("  Описание: ");
            manager.createProject(name, desc);

        } else if (choice == 2) {
            int id      = readInt("  ID на проект: ");
            string name = readLine("  Ново име: ");
            string desc = readLine("  Ново описание: ");
            manager.editProject(id, name, desc);

        } else if (choice == 3) {
            int id = readInt("  ID на проект: ");
            manager.deleteProject(id);

        } else if (choice == 4) {
            manager.displayAll();
        }
    }
}

void menuZadachi(TaskManager& manager) {
    while (true) {
        cout << "\n--- ЗАДАЧИ ---\n"
             << "1. Добавяне на задача\n"
             << "2. Редактиране на задача\n"
             << "3. Изтриване на задача\n"
             << "4. Промяна на статус\n"
             << "5. Филтриране по статус\n"
             << "6. Филтриране по приоритет\n"
             << "7. Сортиране по краен срок\n"
             << "8. Сортиране по приоритет\n"
             << "0. Назад\n";
        int choice = readInt("Избор: ");
        if (choice == 0) break;

        if (choice == 1) {
            int pid     = readInt("  ID на проект: ");
            string name = readLine("  Име на задача: ");
            string desc = readLine("  Описание: ");
            string dead = readLine("  Краен срок (YYYY-MM-DD): ");
            Priority p  = readPriority();
            manager.addTask(pid, name, desc, dead, p);

        } else if (choice == 2) {
            int pid     = readInt("  ID на проект: ");
            int tid     = readInt("  ID на задача: ");
            string name = readLine("  Ново име: ");
            string desc = readLine("  Ново описание: ");
            string dead = readLine("  Нов краен срок (YYYY-MM-DD): ");
            Priority p  = readPriority();
            manager.editTask(pid, tid, name, desc, dead, p);

        } else if (choice == 3) {
            int pid = readInt("  ID на проект: ");
            int tid = readInt("  ID на задача: ");
            manager.deleteTask(pid, tid);

        } else if (choice == 4) {
            int pid = readInt("  ID на проект: ");
            int tid = readInt("  ID на задача: ");
            Status s = readStatus();
            manager.changeStatus(pid, tid, s);

        } else if (choice == 5) {
            int pid  = readInt("  ID на проект: ");
            Status s = readStatus();
            manager.filterByStatus(pid, s);

        } else if (choice == 6) {
            int pid    = readInt("  ID на проект: ");
            Priority p = readPriority();
            manager.filterByPriority(pid, p);

        } else if (choice == 7) {
            int pid = readInt("  ID на проект: ");
            DeadlineSorter sorter;
            manager.displaySorted(pid, sorter);

        } else if (choice == 8) {
            int pid = readInt("  ID на проект: ");
            PrioritySorter sorter;
            manager.displaySorted(pid, sorter);
        }
    }
}

void menuPotrebiteli(TaskManager& manager) {
    while (true) {
        cout << "\n--- ПОТРЕБИТЕЛИ ---\n"
             << "1. Добавяне на потребител\n"
             << "2. Назначаване на потребител към задача\n"
             << "0. Назад\n";
        int choice = readInt("Избор: ");
        if (choice == 0) break;

        if (choice == 1) {
            string name  = readLine("  Име: ");
            string email = readLine("  Имейл: ");
            manager.addUser(name, email);

        } else if (choice == 2) {
            int pid = readInt("  ID на проект: ");
            int tid = readInt("  ID на задача: ");
            int uid = readInt("  ID на потребител: ");
            manager.assignUser(pid, tid, uid);
        }
    }
}

// ── Главно меню ──

int main() {
    cout << "========================================\n"
         << "     МЕНИДЖЪР НА ЗАДАЧИ И ПРОЕКТИ\n"
         << "========================================\n";

    TaskManager manager;

    while (true) {
        cout << "\n=== ГЛАВНО МЕНЮ ===\n"
             << "1. Проекти\n"
             << "2. Задачи\n"
             << "3. Потребители\n"
             << "4. Проверка на крайни срокове\n"
             << "5. История на действията\n"
             << "6. Покажи всичко\n"
             << "0. Изход\n";
        int choice = readInt("Избор: ");

        if      (choice == 0) { cout << "Довиждане!\n"; break; }
        else if (choice == 1) menuProekti(manager);
        else if (choice == 2) menuZadachi(manager);
        else if (choice == 3) menuPotrebiteli(manager);
        else if (choice == 4) manager.checkDeadlines();
        else if (choice == 5) manager.displayHistory();
        else if (choice == 6) manager.displayAll();
        else cout << "[ГРЕШКА] Невалиден избор.\n";
    }

    return 0;
}