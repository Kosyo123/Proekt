#pragma once
#include "BaseEntity.h"
#include "Enums.h"
#include "User.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

class Task : public BaseEntity {
private:
    std::string description;
    std::string deadline;
    Priority    priority;
    Status      status;
    User*       assignedUser;

public:
    Task(int id, const std::string& name, const std::string& description,
         const std::string& deadline, Priority priority);

    // Getters
    const std::string& getDescription()  const;
    const std::string& getDeadline()     const;
    Priority           getPriority()     const;
    Status             getStatus()       const;
    User*              getAssignedUser() const;

    // Setters
    void setDescription(const std::string& d);
    void setDeadline(const std::string& d);
    void setPriority(Priority p);
    void setStatus(Status s);
    void assignTo(User* u);

    // Проверки за краен срок
    bool isOverdue()  const;
    bool isDueSoon()  const;

    void display() const override;
};
