#pragma once
#include "BaseEntity.h"
#include <string>

// ActivityLog наследява BaseEntity — Наследяване + Полиморфизъм
class ActivityLog : public BaseEntity {
private:
    std::string action;
    std::string date;
    int         taskId;

public:
    ActivityLog(int id, const std::string& action, const std::string& date, int taskId);

    const std::string& getAction() const;
    const std::string& getDate()   const;
    int                getTaskId() const;

    void display() const override;
};
