#include "ActivityLog.h"
#include <iostream>

ActivityLog::ActivityLog(int id, const std::string& action, const std::string& date, int taskId)
    : BaseEntity(id, "Log #" + std::to_string(id)),
      action(action), date(date), taskId(taskId) {}

const std::string& ActivityLog::getAction() const { return action; }
const std::string& ActivityLog::getDate()   const { return date; }
int                ActivityLog::getTaskId() const { return taskId; }

void ActivityLog::display() const {
    std::cout << "  [" << date << "] Zadacha #" << taskId << " -- " << action << "\n";
}
