

#include <iostream>
#include <string>
#include <vector>
#include <memory>

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



class BaseEntity {
protected:
    int    id;
    string name;

public:
    BaseEntity(int id, const string& name)
        : id(id), name(name) {}

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
        : BaseEntity(id, name),
          description(description),
          deadline(deadline),
          priority(priority),
          status(Status::TODO),
          assignedUser(nullptr)
    {}

    string   getDescription() const { return description; }
    string   getDeadline()    const { return deadline; }
    Priority getPriority()    const { return priority; }
    Status   getStatus()      const { return status; }
    User*    getAssignedUser()const { return assignedUser; }

    void setStatus(Status s)   { status = s; }
    void assignTo(User* user)  { assignedUser = user; }

    void display() const override {
        cout << "  [Задача #" << id << "] " << name << "\n"
             << "    Описание : " << description << "\n"
             << "    Краен срок: " << deadline << "\n"
             << "    Приоритет: " << priorityToString(priority) << "\n"
             << "    Статус   : " << statusToString(status) << "\n";
        if (assignedUser)
            cout << "    Възложена на: " << assignedUser->getName() << "\n";
        else
            cout << "    Възложена на: —\n";
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

    vector<Task*> getAllTasks() const {
        vector<Task*> result;
        for (const auto& t : tasks)
            result.push_back(t.get());
        return result;
    }

    void display() const override {
        cout << "\n══════════════════════════════════\n"
             << "  Проект #" << id << ": " << name << "\n"
             << "  " << description << "\n"
             << "  Задачи (" << tasks.size() << "):\n"
             << "══════════════════════════════════\n";
        if (tasks.empty()) {
            cout << "  (няма задачи)\n";
        } else {
            for (const auto& t : tasks)
                t->display();
        }
    }

    const string& getDescription() const { return description; }
};



class TaskManager {
private:
    vector<unique_ptr<Project>> projects;
    vector<unique_ptr<User>>    users;
    int nextProjectId = 1;
    int nextTaskId    = 1;
    int nextUserId    = 1;

public:
    
    Project* createProject(const string& name, const string& desc) {
        projects.push_back(make_unique<Project>(nextProjectId++, name, desc));
        cout << "✔ Проект \"" << name << "\" създаден.\n";
        return projects.back().get();
    }

    void addTaskToProject(int projectId, const string& taskName,
                          const string& desc, const string& deadline,
                          Priority priority) {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        p->addTask(nextTaskId++, taskName, desc, deadline, priority);
    }

    void changeTaskStatus(int projectId, int taskId, Status newStatus) {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        Task* t = p->findTask(taskId);
        if (!t) { cout << "✘ Задачата не е намерена.\n"; return; }
        t->setStatus(newStatus);
        cout << "✔ Статусът на \"" << t->getName() << "\" е променен на "
             << statusToString(newStatus) << ".\n";
    }

   
    void displayAll() const {
        if (projects.empty()) { cout << "Няма проекти.\n"; return; }
        for (const auto& p : projects)
            p->display();
    }

    User* addUser(const string& name, const string& email) {
        users.push_back(make_unique<User>(nextUserId++, name, email));
        cout << "✔ Потребител \"" << name << "\" добавен.\n";
        return users.back().get();
    }

    
    void filterByStatus(int projectId, Status s) const {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        auto tasks = p->filterByStatus(s);
        cout << "\n── Задачи със статус " << statusToString(s)
             << " в проект #" << projectId << " ──\n";
        if (tasks.empty()) { cout << "  (няма)\n"; return; }
        for (Task* t : tasks) t->display();
    }

    void filterByPriority(int projectId, Priority pr) const {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        auto tasks = p->filterByPriority(pr);
        cout << "\n── Задачи с приоритет " << priorityToString(pr)
             << " в проект #" << projectId << " ──\n";
        if (tasks.empty()) { cout << "  (няма)\n"; return; }
        for (Task* t : tasks) t->display();
    }

    
    void listTasksByProject(int projectId) const {
        Project* p = findProject(projectId);
        if (!p) { cout << "✘ Проектът не е намерен.\n"; return; }
        auto tasks = p->getAllTasks();
        cout << "\n── Всички задачи на проект #" << projectId
             << " (" << p->getName() << ") ──\n";
        if (tasks.empty()) { cout << "  (няма)\n"; return; }
        for (Task* t : tasks) t->display();
    }

private:
    Project* findProject(int id) const {
        for (const auto& p : projects)
            if (p->getId() == id) return p.get();
        return nullptr;
    }
};



int main() {
    cout << "=== МЕНИДЖЪР НА ЗАДАЧИ — ВЕРСИЯ 2 (5 функционалности) ===\n\n";

    TaskManager manager;

    
    manager.createProject("Уебсайт", "Разработка на корпоративен сайт");
    manager.createProject("Мобилно приложение", "Android/iOS приложение");
    cout << "\n";

   
    manager.addTaskToProject(1, "Дизайн на начална страница",
                             "Figma макети", "2025-06-01", Priority::HIGH);
    manager.addTaskToProject(1, "Backend API",
                             "REST endpoints", "2025-06-15", Priority::MEDIUM);
    manager.addTaskToProject(1, "Тестване",
                             "Unit тестове", "2025-07-01", Priority::LOW);
    manager.addTaskToProject(2, "Login екран",
                             "UI + автентикация", "2025-07-01", Priority::HIGH);
    manager.addTaskToProject(2, "Push известия",
                             "Firebase интеграция", "2025-08-01", Priority::MEDIUM);
    cout << "\n";

    
    manager.changeTaskStatus(1, 1, Status::IN_PROGRESS);
    manager.changeTaskStatus(1, 2, Status::DONE);
    manager.changeTaskStatus(2, 4, Status::IN_PROGRESS);
    cout << "\n";

    
    manager.displayAll();

    
    cout << "\n=== ФИЛТРИРАНЕ ===";
    manager.filterByStatus(1, Status::IN_PROGRESS);
    manager.filterByStatus(1, Status::DONE);
    manager.filterByStatus(1, Status::TODO);
    manager.filterByPriority(1, Priority::HIGH);
    manager.filterByPriority(2, Priority::MEDIUM);
    manager.listTasksByProject(2);

    cout << "\n=== Край ===\n";
    return 0;
}
