#include "TaskManager.h"
#include "Utils.h"
#include <iostream>

void TaskManager::log(int taskId, const std::string& action) {
    logs.push_back(std::make_unique<ActivityLog>(nextLogId++, action, today(), taskId));
}

Project* TaskManager::findProject(int id) {
    for (auto& p : projects)
        if (p->getId() == id) return p.get();
    return nullptr;
}

User* TaskManager::findUser(int id) {
    for (auto& u : users)
        if (u->getId() == id) return u.get();
    return nullptr;
}

// ── Потребители ──────────────────────────

User* TaskManager::addUser(const std::string& name, const std::string& email) {
    users.push_back(std::make_unique<User>(nextUserId++, name, email));
    std::cout << "[OK] Потребител \"" << name << "\" е добавен (id=" << users.back()->getId() << ").\n";
    return users.back().get();
}

// ── 1. Управление на проекти ─────────────

Project* TaskManager::createProject(const std::string& name, const std::string& desc) {
    projects.push_back(std::make_unique<Project>(nextProjectId++, name, desc));
    std::cout << "[OK] Проект \"" << name << "\" е създаден (id=" << projects.back()->getId() << ").\n";
    return projects.back().get();
}

void TaskManager::editProject(int projectId, const std::string& newName, const std::string& newDesc) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return; }
    p->setName(newName);
    p->setDescription(newDesc);
    std::cout << "[OK] Проект #" << projectId << " е редактиран.\n";
}

void TaskManager::deleteProject(int projectId) {
    for (auto it = projects.begin(); it != projects.end(); ++it) {
        if ((*it)->getId() == projectId) {
            std::cout << "[OK] Проект \"" << (*it)->getName() << "\" е изтрит.\n";
            projects.erase(it);
            return;
        }
    }
    std::cout << "[ГРЕШКА] Проектът не е намерен.\n";
}

// ── 2. Управление на задачи ──────────────

Task* TaskManager::addTask(int projectId, const std::string& name, const std::string& desc,
                           const std::string& deadline, Priority priority) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return nullptr; }
    Task* t = p->addTask(nextTaskId++, name, desc, deadline, priority);
    std::cout << "[OK] Задача \"" << name << "\" е добавена (id=" << t->getId() << ").\n";
    log(t->getId(), "Задачата е създадена");
    return t;
}

void TaskManager::editTask(int projectId, int taskId,
                           const std::string& newName, const std::string& newDesc,
                           const std::string& newDeadline, Priority newPriority) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return; }
    Task* t = p->findTask(taskId);
    if (!t) { std::cout << "[ГРЕШКА] Задачата не е намерена.\n"; return; }
    t->setName(newName);
    t->setDescription(newDesc);
    t->setDeadline(newDeadline);
    t->setPriority(newPriority);
    std::cout << "[OK] Задача #" << taskId << " е редактирана.\n";
    log(taskId, "Задачата е редактирана");
}

void TaskManager::deleteTask(int projectId, int taskId) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return; }
    if (p->removeTask(taskId))
        std::cout << "[OK] Задача #" << taskId << " е изтрита.\n";
    else
        std::cout << "[ГРЕШКА] Задачата не е намерена.\n";
}

// ── 3. Статус ────────────────────────────

void TaskManager::changeStatus(int projectId, int taskId, Status newStatus) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return; }
    Task* t = p->findTask(taskId);
    if (!t) { std::cout << "[ГРЕШКА] Задачата не е намерена.\n"; return; }
    t->setStatus(newStatus);
    std::cout << "[OK] Статусът на задача #" << taskId << " -> " << statusToString(newStatus) << ".\n";
    log(taskId, "Статусът е сменен на " + statusToString(newStatus));
}

// ── 5. Филтриране ────────────────────────

void TaskManager::filterByStatus(int projectId, Status s) const {
    const Project* p = nullptr;
    for (const auto& pr : projects)
        if (pr->getId() == projectId) { p = pr.get(); break; }
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return; }

    std::cout << "\n-- Задачи със статус " << statusToString(s)
              << " в \"" << p->getName() << "\" --\n";
    auto filtered = p->filterByStatus(s);
    if (filtered.empty()) std::cout << "  (няма)\n";
    else for (Task* t : filtered) t->display();
}

void TaskManager::filterByPriority(int projectId, Priority pr_) const {
    const Project* p = nullptr;
    for (const auto& pr : projects)
        if (pr->getId() == projectId) { p = pr.get(); break; }
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return; }

    std::cout << "\n-- Задачи с приоритет " << priorityToString(pr_)
              << " в \"" << p->getName() << "\" --\n";
    auto filtered = p->filterByPriority(pr_);
    if (filtered.empty()) std::cout << "  (няма)\n";
    else for (Task* t : filtered) t->display();
}

// ── 5б. Филтриране през всички проекти ──

void TaskManager::filterAllByStatus(Status s) const {
    std::cout << "\n-- Всички задачи със статус " << statusToString(s) << " (всички проекти) --\n";
    bool found = false;
    for (const auto& p : projects) {
        auto filtered = p->filterByStatus(s);
        for (Task* t : filtered) {
            std::cout << "  [Проект: " << p->getName() << "]\n";
            t->display();
            found = true;
        }
    }
    if (!found) std::cout << "  (няма)\n";
}

void TaskManager::displaySorted(int projectId, const Sorter& sorter) const {
    const Project* p = nullptr;
    for (const auto& pr : projects)
        if (pr->getId() == projectId) { p = pr.get(); break; }
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return; }

    std::vector<Task*> tasks = p->getAllTasks();
    sorter.sort(tasks);

    std::cout << "\n-- Сортирани задачи в \"" << p->getName() << "\" --\n";
    for (Task* t : tasks) t->display();
}

// ── 7. Проследяване на крайни срокове ────

void TaskManager::checkDeadlines() const {
    std::cout << "\n== Проследяване на крайни срокове (днес: " << today() << ") ==\n";
    bool found = false;
    for (const auto& p : projects) {
        for (Task* t : p->getAllTasks()) {
            if (t->isOverdue()) {
                std::cout << "  [!] ЗАКЪСНЯЛА  -- [" << p->getName() << "] " << t->getName()
                          << " (срок: " << t->getDeadline() << ")\n";
                found = true;
            } else if (t->isDueSoon()) {
                std::cout << "  [*] ПРЕДСТОИ   -- [" << p->getName() << "] " << t->getName()
                          << " (срок: " << t->getDeadline() << ")\n";
                found = true;
            }
        }
    }
    if (!found) std::cout << "  Всички задачи са навреме.\n";
}

// ── 8. История ───────────────────────────

void TaskManager::displayHistory() const {
    std::cout << "\n== История на действията ==\n";
    if (logs.empty()) { std::cout << "  (няма записи)\n"; return; }
    for (const auto& l : logs)
        l->display();
}

// ── 9. Назначаване на потребител ─────────

void TaskManager::assignUser(int projectId, int taskId, int userId) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ГРЕШКА] Проектът не е намерен.\n"; return; }
    Task* t = p->findTask(taskId);
    if (!t) { std::cout << "[ГРЕШКА] Задачата не е намерена.\n"; return; }
    User* u = findUser(userId);
    if (!u) { std::cout << "[ГРЕШКА] Потребителят не е намерен.\n"; return; }
    t->assignTo(u);
    std::cout << "[OK] Задача #" << taskId << " е възложена на " << u->getName() << ".\n";
    log(taskId, "Възложена на " + u->getName());
}

// ── Показване на всичко ───────────────────

void TaskManager::displayAll() const {
    std::cout << "\n========================================\n"
              << "       ВСИЧКИ ПРОЕКТИ И ЗАДАЧИ\n"
              << "========================================\n";
    if (projects.empty()) { std::cout << "  Няма проекти.\n"; return; }
    for (const auto& p : projects)
        p->display();
}