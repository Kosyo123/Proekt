/*
=============================================================
  МЕНИДЖЪР НА ЗАДАЧИ И ПРОЕКТИ — 6 функционалности
=============================================================
  Надгражда: v5.5
  Добавено:
  ★ 6.0 — Пълно филтриране:
           filterByPriority() + listTasksByProject()
=============================================================
*/

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <ctime>

using namespace std;

enum class Priority { LOW, MEDIUM, HIGH };
enum class Status   { TODO, IN_PROGRESS, DONE };

string priorityToString(Priority p) {
    switch (p) {
        case Priority::LOW:    return "LOW";
        case Priority::MEDIUM: return "MEDIUM";
        case Priority::HIGH:   return "HIGH";
    }
    return "UNKNOWN";
}
string statusToString(Status s) {
    switch (s) {
        case Status::TODO:        return "TODO";
        case Status::IN_PROGRESS: return "IN_PROGRESS";
        case Status::DONE:        return "DONE";
    }
    return "UNKNOWN";
}
int priorityValue(Priority p) {
    switch (p) {
        case Priority::LOW:    return 1;
        case Priority::MEDIUM: return 2;
        case Priority::HIGH:   return 3;
    }
    return 0;
}
string today() {
    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return string(buf);
}
string dateAfterDays(int days) {
    time_t future = time(nullptr) + days * 86400LL;
    tm* ltm = localtime(&future);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return string(buf);
}

class BaseEntity {
protected:
    int    id;
    string name;
public:
    BaseEntity(int id, const string& name) : id(id), name(name) {}
    virtual ~BaseEntity() = default;
    virtual void display() const = 0;
    int    getId()   const { return id; }
    string getName() const { return name; }
};

class User {
private:
    int    id;
    string name;
    string email;
public:
    User(int id, const string& name, const string& email)
        : id(id), name(name), email(email) {}
    int    getId()    const { return id; }
    string getName()  const { return name; }
    string getEmail() const { return email; }
    void display() const {
        cout << "  Потребител: " << name << " (" << email << ")\n";
    }
};

class Task : public BaseEntity {
private:
    string   description;
    string   deadline;
    Priority priority;
    Status   status;
    User*    assignedUser;
public:
    Task(int id, const string& name, const string& description,
         const string& deadline, Priority priority)
        : BaseEntity(id, name), description(description), deadline(deadline),
          priority(priority), status(Status::TODO), assignedUser(nullptr) {}

    string   getDescription() const { return description; }
    string   getDeadline()    const { return deadline; }
    Priority getPriority()    const { return priority; }
    Status   getStatus()      const { return status; }
    User*    getAssignedUser()const { return assignedUser; }
    void setStatus(Status s)        { status = s; }
    void assignTo(User* u)          { assignedUser = u; }

    bool isOverdue() const {
        return !deadline.empty() && deadline < today() && status != Status::DONE;
    }
    bool isDueSoon(int days = 7) const {
        if (deadline.empty() || status == Status::DONE) return false;
        return deadline >= today() && deadline <= dateAfterDays(days);
    }

    void display() const override {
        cout << "  [Задача #" << id << "] " << name << "\n"
             << "    Описание : " << description << "\n"
             << "    Краен срок: " << deadline << "\n"
             << "    Приоритет: " << priorityToString(priority) << "\n"
             << "    Статус   : " << statusToString(status) << "\n";
        if (assignedUser) cout << "    Възложена на: " << assignedUser->getName() << "\n";
        else              cout << "    Възложена на: —\n";
        if (isOverdue())       cout << "    ⚠  ЗАКЪСНЕНА!\n";
        else if (isDueSoon())  cout << "    🔔 Предстои скоро (до 7 дни)!\n";
    }
};

class Project : public BaseEntity {
private:
    string                   description;
    vector<unique_ptr<Task>> tasks;
public:
    Project(int id, const string& name, const string& description)
        : BaseEntity(id, name), description(description) {}

    void addTask(int id, const string& taskName, const string& desc,
                 const string& deadline, Priority priority) {
        tasks.push_back(make_unique<Task>(id, taskName, desc, deadline, priority));
        cout << "✔ Задача \"" << taskName << "\" добавена към проект \"" << name << "\".\n";
    }
    Task* findTask(int taskId) {
        for (auto& t : tasks)
            if (t->getId() == taskId) return t.get();
        return nullptr;
    }
    vector<Task*> getAllTasks() const {
        vector<Task*> r;
        for (const auto& t : tasks) r.push_back(t.get());
        return r;
    }

    // Функционалност 5: Пълно филтриране
    vector<Task*> filterByStatus(Status s) const {
        vector<Task*> r;
        for (const auto& t : tasks)
            if (t->getStatus() == s) r.push_back(t.get());
        return r;
    }
    // ★ 6.0 — Филтриране по ПРИОРИТЕТ
    vector<Task*> filterByPriority(Priority p) const {
        vector<Task*> r;
        for (const auto& t : tasks)
            if (t->getPriority() == p) r.push_back(t.get());
        return r;
    }

    vector<Task*> sortedByDeadline() const {
        vector<Task*> r = getAllTasks();
        sort(r.begin(), r.end(), [](Task* a, Task* b){ return a->getDeadline() < b->getDeadline(); });
        return r;
    }
    vector<Task*> sortedByPriority() const {
        vector<Task*> r = getAllTasks();
        sort(r.begin(), r.end(), [](Task* a, Task* b){ return priorityValue(a->getPriority()) > priorityValue(b->getPriority()); });
        return r;
    }
    vector<Task*> overdueTasks() const {
        vector<Task*> r;
        for (const auto& t : tasks) if (t->isOverdue()) r.push_back(t.get());
        return r;
    }
    vector<Task*> dueSoonTasks(int days = 7) const {
        vector<Task*> r;
        for (const auto& t : tasks) if (t->isDueSoon(days)) r.push_back(t.get());
        return r;
    }

    void display() const override {
        cout << "\n══════════════════════════════════\n"
             << "  Проект #" << id << ": " << name << "\n"
             << "  " << description << "\n"
             << "  Задачи (" << tasks.size() << "):\n"
             << "══════════════════════════════════\n";
        if (tasks.empty()) cout << "  (няма задачи)\n";
        else for (const auto& t : tasks) t->display();
    }
    const string& getDescription() const { return description; }
};

class TaskManager {
private:
    vector<unique_ptr<Project>> projects;
    vector<unique_ptr<User>>    users;
    int nextProjectId = 1, nextTaskId = 1, nextUserId = 1;

    Project* findProject(int id) const {
        for (const auto& p : projects)
            if (p->getId() == id) return p.get();
        return nullptr;
    }
public:
    Project* createProject(const string& name, const string& desc) {
        projects.push_back(make_unique<Project>(nextProjectId++, name, desc));
        cout << "✔ Проект \"" << name << "\" създаден.\n";
        return projects.back().get();
    }
    void addTaskToProject(int pid, const string& taskName, const string& desc,
                          const string& deadline, Priority priority) {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        p->addTask(nextTaskId++, taskName, desc, deadline, priority);
    }
    void changeTaskStatus(int pid, int tid, Status s) {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        Task* t = p->findTask(tid);
        if (!t) { cout << "✘ Задачата не е намерена.\n"; return; }
        t->setStatus(s);
        cout << "✔ Статус на \"" << t->getName() << "\" → " << statusToString(s) << ".\n";
    }
    User* addUser(const string& name, const string& email) {
        users.push_back(make_unique<User>(nextUserId++, name, email));
        cout << "✔ Потребител \"" << name << "\" добавен.\n";
        return users.back().get();
    }
    void displayAll() const {
        if (projects.empty()) { cout << "Няма проекти.\n"; return; }
        for (const auto& p : projects) p->display();
    }

    // Функционалност 5: Пълно филтриране
    void filterByStatus(int pid, Status s) const {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        auto tasks = p->filterByStatus(s);
        cout << "\n── Задачи със статус " << statusToString(s) << " (проект #" << pid << ") ──\n";
        if (tasks.empty()) { cout << "  (няма)\n"; return; }
        for (Task* t : tasks) t->display();
    }
    // ★ 6.0 — Филтриране по приоритет
    void filterByPriority(int pid, Priority pr) const {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        auto tasks = p->filterByPriority(pr);
        cout << "\n── Задачи с приоритет " << priorityToString(pr) << " (проект #" << pid << ") ──\n";
        if (tasks.empty()) { cout << "  (няма)\n"; return; }
        for (Task* t : tasks) t->display();
    }
    // ★ 6.0 — Всички задачи на проект
    void listTasksByProject(int pid) const {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        auto tasks = p->getAllTasks();
        cout << "\n── Всички задачи на \"" << p->getName() << "\" ──\n";
        if (tasks.empty()) { cout << "  (няма)\n"; return; }
        for (Task* t : tasks) t->display();
    }

    void sortByDeadline(int pid) const {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        cout << "\n── По краен срок (проект #" << pid << ") ──\n";
        for (Task* t : p->sortedByDeadline()) t->display();
    }
    void sortByPriority(int pid) const {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        cout << "\n── По приоритет HIGH→LOW (проект #" << pid << ") ──\n";
        for (Task* t : p->sortedByPriority()) t->display();
    }
    void showOverdue(int pid) const {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        auto tasks = p->overdueTasks();
        cout << "\n── ⚠  Закъснели (проект #" << pid << ") ──\n";
        if (tasks.empty()) { cout << "  (няма закъснели)\n"; return; }
        for (Task* t : tasks) t->display();
    }
    void showDueSoon(int pid, int days = 7) const {
        Project* p = findProject(pid);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        auto tasks = p->dueSoonTasks(days);
        cout << "\n── 🔔 Предстоящи в " << days << " дни (проект #" << pid << ") ──\n";
        if (tasks.empty()) { cout << "  (няма)\n"; return; }
        for (Task* t : tasks) t->display();
    }
    // Добави в TaskManager:
    void assignUserToTask(int pid, int tid, User* user) {
        Project* p = findProject(pid);
        if (!p) {
        cout << "✘ Проектът не е намерен.\n";
        return;
        }

        Task* t = p->findTask(tid);
        if (!t) {
            cout << "✘ Задачата не е намерена.\n";
            return;
    }

        t->assignTo(user);

        cout << "✔ Задачата \"" << t->getName()
             << "\" е възложена на "
            << user->getName() << ".\n";
}
};

int main() {
    cout << "=== ВЕРСИЯ 6.0 — Пълно филтриране ===\n\n";
    cout << "  Днес: " << today() << "\n\n";

    TaskManager manager;
    manager.createProject("Уебсайт", "Корпоративен сайт");
    manager.createProject("Мобилно приложение", "Android/iOS");
    manager.addTaskToProject(1, "Дизайн",       "Figma макети",   "2025-01-15", Priority::HIGH);
    manager.addTaskToProject(1, "Backend API",   "REST endpoints", "2026-05-20", Priority::MEDIUM);
    manager.addTaskToProject(1, "Тестване",      "Unit тестове",   "2026-08-01", Priority::LOW);
    manager.addTaskToProject(1, "Документация",  "Технически docs","2026-06-30", Priority::HIGH);
    manager.addTaskToProject(2, "Login екран",   "UI + auth",      "2025-03-01", Priority::HIGH);
    manager.addTaskToProject(2, "Push известия", "Firebase",       "2026-05-19", Priority::MEDIUM);
    cout << "\n";

    manager.changeTaskStatus(1, 1, Status::IN_PROGRESS);
    manager.changeTaskStatus(1, 2, Status::DONE);
    manager.changeTaskStatus(2, 5, Status::IN_PROGRESS);
    cout << "\n";

    manager.displayAll();

    cout << "\n=== ★ ПЪЛНО ФИЛТРИРАНЕ ===";
    manager.filterByStatus(1, Status::TODO);
    manager.filterByStatus(1, Status::IN_PROGRESS);
    manager.filterByStatus(1, Status::DONE);
    manager.filterByPriority(1, Priority::HIGH);
    manager.filterByPriority(1, Priority::LOW);
    manager.listTasksByProject(2);

    cout << "\n=== СОРТИРАНЕ ===";
    manager.sortByDeadline(1);

    cout << "\n=== СРОКОВЕ ===";
    manager.showOverdue(1);
    manager.showDueSoon(1, 7);
    User* u1 = manager.addUser("Ivan", "ivan@gmail.com");
    manager.assignUserToTask(1, 1, u1);

    cout << "\n=== Край ===\n";
    return 0;
}