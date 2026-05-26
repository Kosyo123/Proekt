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
    std::cout << "[OK] Potrebitel \"" << name << "\" dobaven (id=" << users.back()->getId() << ").\n";
    return users.back().get();
}

// ── 1. Управление на проекти ─────────────

Project* TaskManager::createProject(const std::string& name, const std::string& desc) {
    projects.push_back(std::make_unique<Project>(nextProjectId++, name, desc));
    std::cout << "[OK] Proekt \"" << name << "\" sazdaden (id=" << projects.back()->getId() << ").\n";
    return projects.back().get();
}

void TaskManager::editProject(int projectId, const std::string& newName, const std::string& newDesc) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return; }
    p->setName(newName);
    p->setDescription(newDesc);
    std::cout << "[OK] Proekt #" << projectId << " redaktiran.\n";
}

void TaskManager::deleteProject(int projectId) {
    for (auto it = projects.begin(); it != projects.end(); ++it) {
        if ((*it)->getId() == projectId) {
            std::cout << "[OK] Proekt \"" << (*it)->getName() << "\" iztrirt.\n";
            projects.erase(it);
            return;
        }
    }
    std::cout << "[ERR] Proektat ne e nameren.\n";
}

// ── 2. Управление на задачи ──────────────

Task* TaskManager::addTask(int projectId, const std::string& name, const std::string& desc,
                           const std::string& deadline, Priority priority) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return nullptr; }
    Task* t = p->addTask(nextTaskId++, name, desc, deadline, priority);
    std::cout << "[OK] Zadacha \"" << name << "\" dobavena (id=" << t->getId() << ").\n";
    log(t->getId(), "Zadachata e sazdadena");
    return t;
}

void TaskManager::editTask(int projectId, int taskId,
                           const std::string& newName, const std::string& newDesc,
                           const std::string& newDeadline, Priority newPriority) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return; }
    Task* t = p->findTask(taskId);
    if (!t) { std::cout << "[ERR] Zadachata ne e namerena.\n"; return; }
    t->setName(newName);
    t->setDescription(newDesc);
    t->setDeadline(newDeadline);
    t->setPriority(newPriority);
    std::cout << "[OK] Zadacha #" << taskId << " redaktirana.\n";
    log(taskId, "Zadachata e redaktirana");
}

void TaskManager::deleteTask(int projectId, int taskId) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return; }
    if (p->removeTask(taskId))
        std::cout << "[OK] Zadacha #" << taskId << " iztrita.\n";
    else
        std::cout << "[ERR] Zadachata ne e namerena.\n";
}

// ── 3. Статус ────────────────────────────

void TaskManager::changeStatus(int projectId, int taskId, Status newStatus) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return; }
    Task* t = p->findTask(taskId);
    if (!t) { std::cout << "[ERR] Zadachata ne e namerena.\n"; return; }
    t->setStatus(newStatus);
    std::cout << "[OK] Status na zadacha #" << taskId << " -> " << statusToString(newStatus) << ".\n";
    log(taskId, "Status smenen na " + statusToString(newStatus));
}

// ── 5. Филтриране ────────────────────────

void TaskManager::filterByStatus(int projectId, Status s) const {
    const Project* p = nullptr;
    for (const auto& pr : projects)
        if (pr->getId() == projectId) { p = pr.get(); break; }
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return; }

    std::cout << "\n-- Zadachi sus status " << statusToString(s)
              << " v \"" << p->getName() << "\" --\n";
    auto filtered = p->filterByStatus(s);
    if (filtered.empty()) std::cout << "  (nyama)\n";
    else for (Task* t : filtered) t->display();
}

void TaskManager::filterByPriority(int projectId, Priority pr_) const {
    const Project* p = nullptr;
    for (const auto& pr : projects)
        if (pr->getId() == projectId) { p = pr.get(); break; }
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return; }

    std::cout << "\n-- Zadachi s prioritet " << priorityToString(pr_)
              << " v \"" << p->getName() << "\" --\n";
    auto filtered = p->filterByPriority(pr_);
    if (filtered.empty()) std::cout << "  (nyama)\n";
    else for (Task* t : filtered) t->display();
}

// ── 5б. Филтриране през всички проекти ──

void TaskManager::filterAllByStatus(Status s) const {
    std::cout << "\n-- Vsichki zadachi sus status " << statusToString(s) << " (vsikhki proekti) --\n";
    bool found = false;
    for (const auto& p : projects) {
        auto filtered = p->filterByStatus(s);
        for (Task* t : filtered) {
            std::cout << "  [Proekt: " << p->getName() << "]\n";
            t->display();
            found = true;
        }
    }
    if (!found) std::cout << "  (nyama)\n";
}

void TaskManager::displaySorted(int projectId, const Sorter& sorter) const {
    const Project* p = nullptr;
    for (const auto& pr : projects)
        if (pr->getId() == projectId) { p = pr.get(); break; }
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return; }

    std::vector<Task*> tasks = p->getAllTasks();
    sorter.sort(tasks);

    std::cout << "\n-- Sortirani zadachi v \"" << p->getName() << "\" --\n";
    for (Task* t : tasks) t->display();
}

// ── 7. Проследяване на крайни срокове ────

void TaskManager::checkDeadlines() const {
    std::cout << "\n== Prosleqavane na krajni srokove (dnes: " << today() << ") ==\n";
    bool found = false;
    for (const auto& p : projects) {
        for (Task* t : p->getAllTasks()) {
            if (t->isOverdue()) {
                std::cout << "  [!] ZAKASNQLA  -- [" << p->getName() << "] " << t->getName()
                          << " (srok: " << t->getDeadline() << ")\n";
                found = true;
            } else if (t->isDueSoon()) {
                std::cout << "  [*] PREDSTOI   -- [" << p->getName() << "] " << t->getName()
                          << " (srok: " << t->getDeadline() << ")\n";
                found = true;
            }
        }
    }
    if (!found) std::cout << "  Vsichki zadachi sa navreme.\n";
}

// ── 8. История ───────────────────────────

void TaskManager::displayHistory() const {
    std::cout << "\n== Istoriq na dejstviata ==\n";
    if (logs.empty()) { std::cout << "  (nyama zapisi)\n"; return; }
    for (const auto& l : logs)
        l->display();
}

// ── 9. Назначаване на потребител ─────────

void TaskManager::assignUser(int projectId, int taskId, int userId) {
    Project* p = findProject(projectId);
    if (!p) { std::cout << "[ERR] Proektat ne e nameren.\n"; return; }
    Task* t = p->findTask(taskId);
    if (!t) { std::cout << "[ERR] Zadachata ne e namerena.\n"; return; }
    User* u = findUser(userId);
    if (!u) { std::cout << "[ERR] Potrebitelat ne e nameren.\n"; return; }
    t->assignTo(u);
    std::cout << "[OK] Zadacha #" << taskId << " vazlojhena na " << u->getName() << ".\n";
    log(taskId, "Vazlozhena na " + u->getName());
}

// ── Показване на всичко ───────────────────

void TaskManager::displayAll() const {
    std::cout << "\n========================================\n"
              << "       VSICHKI PROEKTI I ZADACHI\n"
              << "========================================\n";
    if (projects.empty()) { std::cout << "  Nyama proekti.\n"; return; }
    for (const auto& p : projects)
        p->display();
}
