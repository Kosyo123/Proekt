/*
=============================================================
  TASK MANAGER SYSTEM — VERSION 8.5
=============================================================
  ФУНКЦИОНАЛНОСТИ:
  ✔ Управление на проекти
  ✔ Управление на задачи
  ✔ Статуси
  ✔ Приоритети
  ✔ Филтриране
  ✔ Сортиране
  ✔ Следене на срокове
  ✔ Назначаване на потребители
  ✔ Пълна HISTORY система
  ✔ РЕДАКТИРАНЕ НА ЗАДАЧИ
=============================================================
*/

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <ctime>

using namespace std;

/* =========================================================
   ENUMS
========================================================= */

enum class Priority {
    LOW,
    MEDIUM,
    HIGH
};

enum class Status {
    TODO,
    IN_PROGRESS,
    DONE
};

/* =========================================================
   HELPERS
========================================================= */

string priorityToString(Priority p) {

    switch (p) {

        case Priority::LOW:
            return "LOW";

        case Priority::MEDIUM:
            return "MEDIUM";

        case Priority::HIGH:
            return "HIGH";
    }

    return "UNKNOWN";
}

string statusToString(Status s) {

    switch (s) {

        case Status::TODO:
            return "TODO";

        case Status::IN_PROGRESS:
            return "IN_PROGRESS";

        case Status::DONE:
            return "DONE";
    }

    return "UNKNOWN";
}

int priorityValue(Priority p) {

    switch (p) {

        case Priority::LOW:
            return 1;

        case Priority::MEDIUM:
            return 2;

        case Priority::HIGH:
            return 3;
    }

    return 0;
}

string today() {

    time_t now = time(nullptr);

    tm* ltm = localtime(&now);

    char buf[11];

    strftime(
        buf,
        sizeof(buf),
        "%Y-%m-%d",
        ltm
    );

    return string(buf);
}

string dateAfterDays(int days) {

    time_t future =
        time(nullptr)
        + days * 86400LL;

    tm* ltm = localtime(&future);

    char buf[11];

    strftime(
        buf,
        sizeof(buf),
        "%Y-%m-%d",
        ltm
    );

    return string(buf);
}

/* =========================================================
   HISTORY
========================================================= */

class HistoryLog {

private:

    vector<string> logs;

public:

    void add(const string& msg) {

        string fullLog =
            "[" + today() + "] " + msg;

        logs.push_back(fullLog);
    }

    void show() const {

        cout << "\n========== HISTORY ==========\n";

        if (logs.empty()) {

            cout << "Няма история.\n";
            return;
        }

        for (const auto& log : logs) {

            cout << log << endl;
        }
    }
};

/* =========================================================
   BASE ENTITY
========================================================= */

class BaseEntity {

protected:

    int id;
    string name;

public:

    BaseEntity(
        int id,
        const string& name
    )
        : id(id),
          name(name) {}

    virtual ~BaseEntity() = default;

    virtual void display() const = 0;

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }
};

/* =========================================================
   USER
========================================================= */

class User {

private:

    int id;
    string name;
    string email;

public:

    User(
        int id,
        const string& name,
        const string& email
    )
        : id(id),
          name(name),
          email(email) {}

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    string getEmail() const {
        return email;
    }
};

/* =========================================================
   TASK
========================================================= */

class Task : public BaseEntity {

private:

    string description;
    string deadline;

    Priority priority;
    Status status;

    User* assignedUser;

public:

    Task(
        int id,
        const string& name,
        const string& description,
        const string& deadline,
        Priority priority
    )
        : BaseEntity(id, name),
          description(description),
          deadline(deadline),
          priority(priority),
          status(Status::TODO),
          assignedUser(nullptr) {}

    string getDescription() const {
        return description;
    }

    string getDeadline() const {
        return deadline;
    }

    Priority getPriority() const {
        return priority;
    }

    Status getStatus() const {
        return status;
    }

    User* getAssignedUser() const {
        return assignedUser;
    }

    void setStatus(Status s) {
        status = s;
    }

    void assignTo(User* u) {
        assignedUser = u;
    }

    void removeAssignedUser() {
        assignedUser = nullptr;
    }

    /* ===== VERSION 8.5 ===== */

    void editDescription(
        const string& newDesc
    ) {
        description = newDesc;
    }

    void editDeadline(
        const string& newDeadline
    ) {
        deadline = newDeadline;
    }

    bool isOverdue() const {

        return
            !deadline.empty()
            &&
            deadline < today()
            &&
            status != Status::DONE;
    }

    bool isDueSoon(int days = 7) const {

        if (
            deadline.empty()
            ||
            status == Status::DONE
        ) {
            return false;
        }

        return
            deadline >= today()
            &&
            deadline <= dateAfterDays(days);
    }

    void display() const override {

        cout << "\n[ЗАДАЧА #" << id << "] "
             << name << endl;

        cout << "Описание: "
             << description << endl;

        cout << "Краен срок: "
             << deadline << endl;

        cout << "Приоритет: "
             << priorityToString(priority)
             << endl;

        cout << "Статус: "
             << statusToString(status)
             << endl;

        if (assignedUser) {

            cout << "Възложена на: "
                 << assignedUser->getName()
                 << endl;
        }
        else {

            cout << "Възложена на: ---\n";
        }

        if (isOverdue()) {

            cout << "⚠ Закъсняла задача!\n";
        }
        else if (isDueSoon()) {

            cout << "🔔 Предстои скоро!\n";
        }
    }
};

/* =========================================================
   PROJECT
========================================================= */

class Project : public BaseEntity {

private:

    string description;

    vector<unique_ptr<Task>> tasks;

public:

    Project(
        int id,
        const string& name,
        const string& description
    )
        : BaseEntity(id, name),
          description(description) {}

    void addTask(
        int id,
        const string& taskName,
        const string& desc,
        const string& deadline,
        Priority priority
    ) {

        tasks.push_back(

            make_unique<Task>(

                id,
                taskName,
                desc,
                deadline,
                priority
            )
        );

        cout
            << "✔ Добавена задача: "
            << taskName
            << endl;
    }

    Task* findTask(int taskId) {

        for (auto& t : tasks) {

            if (t->getId() == taskId) {

                return t.get();
            }
        }

        return nullptr;
    }

    vector<Task*> getAllTasks() const {

        vector<Task*> result;

        for (const auto& t : tasks) {

            result.push_back(t.get());
        }

        return result;
    }

    void display() const override {

        cout
            << "\n=============================\n";

        cout
            << "Проект #"
            << id
            << ": "
            << name
            << endl;

        cout
            << description
            << endl;

        cout
            << "=============================\n";

        if (tasks.empty()) {

            cout << "Няма задачи.\n";
        }

        for (const auto& t : tasks) {

            t->display();
        }
    }
};

/* =========================================================
   TASK MANAGER
========================================================= */

class TaskManager {

private:

    vector<unique_ptr<Project>> projects;

    vector<unique_ptr<User>> users;

    int nextProjectId = 1;
    int nextTaskId = 1;
    int nextUserId = 1;

    HistoryLog history;

    Project* findProject(int id) const {

        for (const auto& p : projects) {

            if (p->getId() == id) {

                return p.get();
            }
        }

        return nullptr;
    }

public:

    Project* createProject(
        const string& name,
        const string& desc
    ) {

        projects.push_back(

            make_unique<Project>(

                nextProjectId++,
                name,
                desc
            )
        );

        history.add(
            "Създаден проект: " + name
        );

        cout
            << "✔ Създаден проект: "
            << name
            << endl;

        return projects.back().get();
    }

    void addTaskToProject(
        int pid,
        const string& taskName,
        const string& desc,
        const string& deadline,
        Priority priority
    ) {

        Project* p =
            findProject(pid);

        if (!p) {

            cout
                << "✘ Проектът не е намерен.\n";

            return;
        }

        p->addTask(

            nextTaskId++,
            taskName,
            desc,
            deadline,
            priority
        );

        history.add(
            "Добавена задача: " + taskName
        );
    }

    void changeTaskStatus(
        int pid,
        int tid,
        Status s
    ) {

        Project* p =
            findProject(pid);

        if (!p) {

            cout
                << "✘ Проектът не е намерен.\n";

            return;
        }

        Task* t =
            p->findTask(tid);

        if (!t) {

            cout
                << "✘ Задачата не е намерена.\n";

            return;
        }

        t->setStatus(s);

        history.add(

            "Променен статус: "
            + t->getName()
            + " -> "
            + statusToString(s)
        );

        cout
            << "✔ Статусът е обновен.\n";
    }

    User* addUser(
        const string& name,
        const string& email
    ) {

        users.push_back(

            make_unique<User>(

                nextUserId++,
                name,
                email
            )
        );

        history.add(
            "Добавен потребител: " + name
        );

        cout
            << "✔ Добавен потребител: "
            << name
            << endl;

        return users.back().get();
    }

    void assignUserToTask(
        int pid,
        int tid,
        User* user
    ) {

        Project* p =
            findProject(pid);

        if (!p) {

            cout
                << "✘ Проектът не е намерен.\n";

            return;
        }

        Task* t =
            p->findTask(tid);

        if (!t) {

            cout
                << "✘ Задачата не е намерена.\n";

            return;
        }

        t->assignTo(user);

        history.add(

            "Задача \""
            + t->getName()
            + "\" е възложена на "
            + user->getName()
        );

        cout
            << "✔ Задачата е възложена.\n";
    }

    /* ===== VERSION 8.5 ===== */

    void editTask(
        int pid,
        int tid,
        const string& newDesc,
        const string& newDeadline
    ) {

        Project* p =
            findProject(pid);

        if (!p) {

            cout
                << "✘ Проектът не е намерен.\n";

            return;
        }

        Task* t =
            p->findTask(tid);

        if (!t) {

            cout
                << "✘ Задачата не е намерена.\n";

            return;
        }

        t->editDescription(newDesc);

        t->editDeadline(newDeadline);

        history.add(

            "Редактирана задача: "
            + t->getName()
        );

        cout
            << "✔ Задачата е редактирана.\n";
    }

    void displayAll() const {

        if (projects.empty()) {

            cout << "Няма проекти.\n";
            return;
        }

        for (const auto& p : projects) {

            p->display();
        }
    }

    void showHistory() const {

        history.show();
    }
};

/* =========================================================
   MAIN
========================================================= */

int main() {

    cout
        << "=== TASK MANAGER VERSION 8.5 ===\n\n";

    TaskManager manager;

    manager.createProject(
        "Уебсайт",
        "Корпоративен сайт"
    );

    manager.addTaskToProject(
        1,
        "Frontend",
        "React UI",
        "2026-05-25",
        Priority::HIGH
    );

    manager.addTaskToProject(
        1,
        "Backend",
        "REST API",
        "2026-05-30",
        Priority::MEDIUM
    );

    User* u1 =
        manager.addUser(
            "Ivan",
            "ivan@gmail.com"
        );

    manager.assignUserToTask(
        1,
        1,
        u1
    );

    manager.changeTaskStatus(
        1,
        1,
        Status::IN_PROGRESS
    );

    /* ===== NEW IN 8.5 ===== */

    manager.editTask(
        1,
        1,
        "Updated React UI Design",
        "2026-06-15"
    );

    cout
        << "\n========== ВСИЧКИ ПРОЕКТИ ==========\n";

    manager.displayAll();

    manager.showHistory();

    cout
        << "\n=== КРАЙ ===\n";

    return 0;
}