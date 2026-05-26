#pragma once
#include "Project.h"
#include "User.h"
#include "ActivityLog.h"
#include "Sorter.h"
#include <vector>
#include <memory>
#include <string>

// Централен контролер — управлява всички обекти
class TaskManager {
private:
    std::vector<std::unique_ptr<Project>>     projects;
    std::vector<std::unique_ptr<User>>        users;
    std::vector<std::unique_ptr<ActivityLog>> logs;

    int nextProjectId = 1;
    int nextTaskId    = 1;
    int nextUserId    = 1;
    int nextLogId     = 1;

    void     log(int taskId, const std::string& action);
    Project* findProject(int id);
    User*    findUser(int id);

public:
    // Потребители
    User* addUser(const std::string& name, const std::string& email);

    // 1. Управление на проекти
    Project* createProject(const std::string& name, const std::string& desc);
    void     editProject(int projectId, const std::string& newName, const std::string& newDesc);
    void     deleteProject(int projectId);

    // 2. Управление на задачи
    Task* addTask(int projectId, const std::string& name, const std::string& desc,
                  const std::string& deadline, Priority priority);
    void  editTask(int projectId, int taskId,
                   const std::string& newName, const std::string& newDesc,
                   const std::string& newDeadline, Priority newPriority);
    void  deleteTask(int projectId, int taskId);

    // 3. Статус
    void changeStatus(int projectId, int taskId, Status newStatus);

    // 5. Филтриране
    void filterByStatus(int projectId, Status s)     const;
    void filterByPriority(int projectId, Priority p) const;
    void filterAllByStatus(Status s)                 const;  // през всички проекти

    // 6. Сортиране
    void displaySorted(int projectId, const Sorter& sorter) const;

    // 7. Проследяване на крайни срокове
    void checkDeadlines() const;

    // 8. История
    void displayHistory() const;

    // 9. Назначаване на потребител
    void assignUser(int projectId, int taskId, int userId);

    // Показване на всичко
    void displayAll() const;
};
