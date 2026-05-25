

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;


string today() {
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    ostringstream oss;
    oss << (1900 + t->tm_year) << "-"
        << setw(2) << setfill('0') << (1 + t->tm_mon) << "-"
        << setw(2) << setfill('0') << t->tm_mday;
    return oss.str();
}

bool dateIsBefore(const string& a, const string& b) { return a < b; }


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
        case Priority::LOW:    return 0;
        case Priority::MEDIUM: return 1;
        case Priority::HIGH:   return 2;
    }
    return 0;
}

// ─────────────────────────────────────────
// Абстрактен базов клас — BaseEntity
// (Абстракция + Полиморфизъм)
// ─────────────────────────────────────────

class BaseEntity {
protected:
    int    id;
    string name;

public:
    BaseEntity(int id, const string& name)
        : id(id), name(name) {}

    virtual ~BaseEntity() = default;

    // Чисто виртуален метод — абстракция
    virtual void display() const = 0;

    int           getId()   const { return id; }
    const string& getName() const { return name; }
    void          setName(const string& n) { name = n; }
};

// ─────────────────────────────────────────
// Клас User (Потребител)
// ─────────────────────────────────────────

class User {
private:
    int    id;
    string name;
    string email;

public:
    User(int id, const string& name, const string& email)
        : id(id), name(name), email(email) {}

    int           getId()    const { return id; }
    const string& getName()  const { return name; }
    const string& getEmail() const { return email; }
};

// ─────────────────────────────────────────
// Forward declaration
// ─────────────────────────────────────────
class Task;

// ─────────────────────────────────────────
// Клас ActivityLog — наследява BaseEntity
// (Наследяване + Полиморфизъм)
// ─────────────────────────────────────────

class ActivityLog : public BaseEntity {
private:
    string action;
    string date;
    int    taskId;   // съхраняваме id вместо указател (избягваме dangling)

public:
    ActivityLog(int id, const string& action, const string& date, int taskId)
        : BaseEntity(id, "Log #" + to_string(id)),
          action(action), date(date), taskId(taskId) {}

    const string& getAction() const { return action; }
    const string& getDate()   const { return date; }
    int           getTaskId() const { return taskId; }

    void display() const override {
        cout << "  [" << date << "] Задача #" << taskId << " — " << action << "\n";
    }
};

// ─────────────────────────────────────────
// Клас Task — наследява BaseEntity
// (Наследяване + Енкапсулация + Полиморфизъм)
// ─────────────────────────────────────────

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
        : BaseEntity(id, name),
          description(description),
          deadline(deadline),
          priority(priority),
          status(Status::TODO),
          assignedUser(nullptr)
    {}

    // Getters
    const string& getDescription() const { return description; }
    const string& getDeadline()    const { return deadline; }
    Priority      getPriority()    const { return priority; }
    Status        getStatus()      const { return status; }
    User*         getAssignedUser()const { return assignedUser; }

    // Setters
    void setDescription(const string& d) { description = d; }
    void setDeadline(const string& d)    { deadline = d; }
    void setPriority(Priority p)         { priority = p; }
    void setStatus(Status s)             { status = s; }
    void assignTo(User* u)               { assignedUser = u; }

    // Проверки за краен срок
    bool isOverdue() const {
        return dateIsBefore(deadline, today()) && status != Status::DONE;
    }

    bool isDueSoon() const {
        // "скоро" = в рамките на 3 дни; бърза проверка чрез date string
        if (status == Status::DONE || isOverdue()) return false;
        // изчисляваме target = today + 3 дни
        time_t now = time(nullptr);
        now += 3 * 24 * 3600;
        tm* t = localtime(&now);
        ostringstream oss;
        oss << (1900 + t->tm_year) << "-"
            << setw(2) << setfill('0') << (1 + t->tm_mon) << "-"
            << setw(2) << setfill('0') << t->tm_mday;
        string limit = oss.str();
        return !dateIsBefore(limit, deadline) && !dateIsBefore(deadline, today());
    }

    void display() const override {
        string flag = "";
        if (isOverdue())      flag = " ⚠ ЗАКЪСНЯЛА";
        else if (isDueSoon()) flag = " ⏰ ПРЕДСТОИ СКОРО";

        cout << "  ┌─ Задача #" << id << ": " << name << flag << "\n"
             << "  │  Описание : " << description << "\n"
             << "  │  Срок     : " << deadline << "\n"
             << "  │  Приоритет: " << priorityToString(priority) << "\n"
             << "  │  Статус   : " << statusToString(status) << "\n";
        if (assignedUser)
            cout << "  └  Отговорник: " << assignedUser->getName()
                 << " <" << assignedUser->getEmail() << ">\n";
        else
            cout << "  └  Отговорник: —\n";
    }
};

// ─────────────────────────────────────────
// Абстрактна стратегия за сортиране — Sorter
// (Абстракция + Полиморфизъм)
// ─────────────────────────────────────────

class Sorter {
public:
    virtual ~Sorter() = default;
    virtual bool compare(const Task* a, const Task* b) const = 0;

    void sort(vector<Task*>& tasks) const {
        std::sort(tasks.begin(), tasks.end(),
                  [this](const Task* a, const Task* b) {
                      return this->compare(a, b);
                  });
    }
};

// Сортиране по краен срок (ascending)
class DeadlineSorter : public Sorter {
public:
    bool compare(const Task* a, const Task* b) const override {
        return dateIsBefore(a->getDeadline(), b->getDeadline());
    }
};

// Сортиране по приоритет (HIGH → LOW)
class PrioritySorter : public Sorter {
public:
    bool compare(const Task* a, const Task* b) const override {
        return priorityValue(a->getPriority()) > priorityValue(b->getPriority());
    }
};

// ─────────────────────────────────────────
// Клас Project — наследява BaseEntity
// (Наследяване + Енкапсулация + Полиморфизъм)
// ─────────────────────────────────────────

class Project : public BaseEntity {
private:
    string                   description;
    vector<unique_ptr<Task>> tasks;

public:
    Project(int id, const string& name, const string& desc)
        : BaseEntity(id, name), description(desc) {}

    const string& getDescription() const { return description; }
    void setDescription(const string& d)  { description = d; }

    // Добавяне на задача
    Task* addTask(int id, const string& taskName, const string& desc,
                  const string& deadline, Priority priority) {
        tasks.push_back(make_unique<Task>(id, taskName, desc, deadline, priority));
        return tasks.back().get();
    }

    // Изтриване на задача по id
    bool removeTask(int taskId) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if ((*it)->getId() == taskId) {
                tasks.erase(it);
                return true;
            }
        }
        return false;
    }

    Task* findTask(int taskId) {
        for (auto& t : tasks)
            if (t->getId() == taskId) return t.get();
        return nullptr;
    }

    // Всички задачи като вектор от суровите указатели
    vector<Task*> getAllTasks() const {
        vector<Task*> result;
        for (const auto& t : tasks) result.push_back(t.get());
        return result;
    }

    // 5. Филтриране
    vector<Task*> filterByStatus(Status s) const {
        vector<Task*> result;
        for (const auto& t : tasks)
            if (t->getStatus() == s) result.push_back(t.get());
        return result;
    }

    vector<Task*> filterByPriority(Priority p) const {
        vector<Task*> result;
        for (const auto& t : tasks)
            if (t->getPriority() == p) result.push_back(t.get());
        return result;
    }

    // Полиморфизъм — override
    void display() const override {
        cout << "\n╔══════════════════════════════════════╗\n"
             << "  Проект #" << id << ": " << name << "\n"
             << "  " << description << "\n"
             << "  Задачи: " << tasks.size() << "\n"
             << "╚══════════════════════════════════════╝\n";
        if (tasks.empty())
            cout << "  (няма задачи)\n";
        else
            for (const auto& t : tasks)
                t->display();
    }
};

// ─────────────────────────────────────────
// Клас TaskManager — централен контролер
// ─────────────────────────────────────────

class TaskManager {
private:
    vector<unique_ptr<Project>>     projects;
    vector<unique_ptr<User>>        users;
    vector<unique_ptr<ActivityLog>> logs;

    int nextProjectId = 1;
    int nextTaskId    = 1;
    int nextUserId    = 1;
    int nextLogId     = 1;

    void log(int taskId, const string& action) {
        logs.push_back(make_unique<ActivityLog>(nextLogId++, action, today(), taskId));
    }

    Project* findProject(int id) {
        for (auto& p : projects)
            if (p->getId() == id) return p.get();
        return nullptr;
    }

    User* findUser(int id) {
        for (auto& u : users)
            if (u->getId() == id) return u.get();
        return nullptr;
    }

public:

    // ── Потребители ──────────────────────────

    User* addUser(const string& name, const string& email) {
        users.push_back(make_unique<User>(nextUserId++, name, email));
        cout << "✔ Потребител \"" << name << "\" добавен (id=" << users.back()->getId() << ").\n";
        return users.back().get();
    }

    // ── 1. Управление на проекти ─────────────

    Project* createProject(const string& name, const string& desc) {
        projects.push_back(make_unique<Project>(nextProjectId++, name, desc));
        cout << "✔ Проект \"" << name << "\" създаден (id=" << projects.back()->getId() << ").\n";
        return projects.back().get();
    }

    void editProject(int projectId, const string& newName, const string& newDesc) {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        p->setName(newName);
        p->setDescription(newDesc);
        cout << "✔ Проект #" << projectId << " редактиран.\n";
    }

    void deleteProject(int projectId) {
        for (auto it = projects.begin(); it != projects.end(); ++it) {
            if ((*it)->getId() == projectId) {
                cout << "✔ Проект \"" << (*it)->getName() << "\" изтрит.\n";
                projects.erase(it);
                return;
            }
        }
        cout << "✘ Проектът не е намерен.\n";
    }

    // ── 2. Управление на задачи ──────────────

    Task* addTask(int projectId, const string& name, const string& desc,
                  const string& deadline, Priority priority) {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return nullptr; }
        Task* t = p->addTask(nextTaskId++, name, desc, deadline, priority);
        cout << "✔ Задача \"" << name << "\" добавена (id=" << t->getId() << ").\n";
        log(t->getId(), "Задачата е създадена");
        return t;
    }

    void editTask(int projectId, int taskId,
                  const string& newName, const string& newDesc,
                  const string& newDeadline, Priority newPriority) {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        Task* t = p->findTask(taskId);
        if (!t) { cout << "✘ Задачата не е намерена.\n"; return; }
        t->setName(newName);
        t->setDescription(newDesc);
        t->setDeadline(newDeadline);
        t->setPriority(newPriority);
        cout << "✔ Задача #" << taskId << " редактирана.\n";
        log(taskId, "Задачата е редактирана");
    }

    void deleteTask(int projectId, int taskId) {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        if (p->removeTask(taskId))
            cout << "✔ Задача #" << taskId << " изтрита.\n";
        else
            cout << "✘ Задачата не е намерена.\n";
    }

    // ── 3. Статус на задачите ────────────────

    void changeStatus(int projectId, int taskId, Status newStatus) {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        Task* t = p->findTask(taskId);
        if (!t) { cout << "✘ Задачата не е намерена.\n"; return; }
        t->setStatus(newStatus);
        cout << "✔ Статус на задача #" << taskId << " → " << statusToString(newStatus) << ".\n";
        log(taskId, "Статус сменен на " + statusToString(newStatus));
    }

    // ── 4. Приоритет — зададен при addTask/editTask

    // ── 5. Филтриране ────────────────────────

    void filterByStatus(int projectId, Status s) const {
        const Project* p = nullptr;
        for (const auto& pr : projects)
            if (pr->getId() == projectId) { p = pr.get(); break; }
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }

        cout << "\n── Задачи със статус " << statusToString(s)
             << " в \"" << p->getName() << "\" ──\n";
        auto filtered = p->filterByStatus(s);
        if (filtered.empty()) cout << "  (няма)\n";
        else for (Task* t : filtered) t->display();
    }

    void filterByPriority(int projectId, Priority pr_) const {
        const Project* p = nullptr;
        for (const auto& pr : projects)
            if (pr->getId() == projectId) { p = pr.get(); break; }
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }

        cout << "\n── Задачи с приоритет " << priorityToString(pr_)
             << " в \"" << p->getName() << "\" ──\n";
        auto filtered = p->filterByPriority(pr_);
        if (filtered.empty()) cout << "  (няма)\n";
        else for (Task* t : filtered) t->display();
    }

    // ── 6. Сортиране (полиморфизъм чрез Sorter) ──

    void displaySorted(int projectId, const Sorter& sorter) const {
        const Project* p = nullptr;
        for (const auto& pr : projects)
            if (pr->getId() == projectId) { p = pr.get(); break; }
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }

        vector<Task*> tasks = p->getAllTasks();
        sorter.sort(tasks);   // полиморфизъм — DeadlineSorter или PrioritySorter

        cout << "\n── Сортирани задачи в \"" << p->getName() << "\" ──\n";
        for (Task* t : tasks) t->display();
    }

    // ── 7. Проследяване на крайни срокове ────

    void checkDeadlines() const {
        cout << "\n══ Проследяване на крайни срокове (днес: " << today() << ") ══\n";
        bool found = false;
        for (const auto& p : projects) {
            for (Task* t : p->getAllTasks()) {
                if (t->isOverdue()) {
                    cout << "  ⚠  ЗАКЪСНЯЛА  — [" << p->getName() << "] " << t->getName()
                         << " (срок: " << t->getDeadline() << ")\n";
                    found = true;
                } else if (t->isDueSoon()) {
                    cout << "  ⏰ ПРЕДСТОИ   — [" << p->getName() << "] " << t->getName()
                         << " (срок: " << t->getDeadline() << ")\n";
                    found = true;
                }
            }
        }
        if (!found) cout << "  Всички задачи са навреме.\n";
    }

    // ── 8. История на действията ─────────────

    void displayHistory() const {
        cout << "\n══ История на действията ══\n";
        if (logs.empty()) { cout << "  (няма записи)\n"; return; }
        for (const auto& l : logs)
            l->display();   // полиморфизъм
    }

    // ── Назначаване на потребител ─────────────

    void assignUser(int projectId, int taskId, int userId) {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        Task* t = p->findTask(taskId);
        if (!t) { cout << "✘ Задачата не е намерена.\n"; return; }
        User* u = findUser(userId);
        if (!u) { cout << "✘ Потребителят не е намерен.\n"; return; }
        t->assignTo(u);
        cout << "✔ Задача #" << taskId << " възложена на " << u->getName() << ".\n";
        log(taskId, "Възложена на " + u->getName());
    }

    // ── Показване на всичко ───────────────────

    void displayAll() const {
        cout << "\n════════════════════════════════════════\n"
             << "         ВСИЧКИ ПРОЕКТИ И ЗАДАЧИ\n"
             << "════════════════════════════════════════\n";
        if (projects.empty()) { cout << "  Няма проекти.\n"; return; }
        for (const auto& p : projects)
            p->display();   // виртуален метод — полиморфизъм
    }
};

// ─────────────────────────────────────────
// main — пълна демонстрация
// ─────────────────────────────────────────

int main() {
    cout << "=== МЕНИДЖЪР НА ЗАДАЧИ — ВЕРСИЯ 2 (пълна) ===\n\n";

    TaskManager manager;

    // Потребители
    User* ana  = manager.addUser("Ана Иванова",  "ana@example.com");
    User* ivan = manager.addUser("Иван Петров",  "ivan@example.com");
    cout << "\n";

    // 1. Управление на проекти
    manager.createProject("Уебсайт",            "Корпоративен сайт");
    manager.createProject("Мобилно приложение", "Android и iOS");
    manager.editProject(1, "Уебсайт v2", "Редизайн на корпоративен сайт");
    cout << "\n";

    // 2. Добавяне на задачи
    // Използваме минали дати за да демонстрираме закъсняла задача
    Task* t1 = manager.addTask(1, "Дизайн",      "Figma макети",      "2024-01-10", Priority::HIGH);
    Task* t2 = manager.addTask(1, "Backend API", "REST endpoints",    "2025-12-31", Priority::MEDIUM);
    Task* t3 = manager.addTask(2, "Login екран", "UI + auth",         "2025-06-30", Priority::HIGH);
    Task* t4 = manager.addTask(2, "Push нотиф.", "Firebase setup",    "2025-07-15", Priority::LOW);
    cout << "\n";

    // Назначаване на потребители
    manager.assignUser(1, t1->getId(), ana->getId());
    manager.assignUser(1, t2->getId(), ivan->getId());
    cout << "\n";

    
    manager.changeStatus(1, t1->getId(), Status::IN_PROGRESS);
    manager.changeStatus(2, t3->getId(), Status::DONE);
    cout << "\n";

    
    manager.editTask(1, t2->getId(), "Backend API v2", "GraphQL + REST", "2025-11-30", Priority::HIGH);
    manager.deleteTask(2, t4->getId());
    cout << "\n";

    
    manager.filterByStatus(1, Status::IN_PROGRESS);
    manager.filterByPriority(1, Priority::HIGH);
    cout << "\n";

    
    DeadlineSorter  bySroc;
    PrioritySorter  byPriority;

    cout << "\n>> Сортиране по краен срок:\n";
    manager.displaySorted(1, bySroc);

    cout << "\n>> Сортиране по приоритет:\n";
    manager.displaySorted(1, byPriority);

    
    manager.checkDeadlines();

    
    manager.displayHistory();

    
    manager.displayAll();

    cout << "\n=== Край ===\n";
    return 0;
}
