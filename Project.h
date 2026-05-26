#pragma once
#include "BaseEntity.h"
#include "Task.h"
#include "Enums.h"
#include <vector>
#include <memory>
#include <string>

// Project наследява BaseEntity — Наследяване + Енкапсулация + Полиморфизъм
class Project : public BaseEntity {
private:
    std::string                   description;
    std::vector<std::unique_ptr<Task>> tasks;

public:
    Project(int id, const std::string& name, const std::string& desc);

    const std::string& getDescription() const;
    void               setDescription(const std::string& d);

    Task* addTask(int id, const std::string& taskName, const std::string& desc,
                  const std::string& deadline, Priority priority);

    bool  removeTask(int taskId);
    Task* findTask(int taskId);

    std::vector<Task*> getAllTasks()          const;
    std::vector<Task*> filterByStatus(Status s)    const;
    std::vector<Task*> filterByPriority(Priority p) const;

    void display() const override;
};
