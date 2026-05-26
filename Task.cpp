#include "Task.h"
#include "Utils.h"
#include <iostream>

Task::Task(int id, const std::string& name, const std::string& description,
           const std::string& deadline, Priority priority)
    : BaseEntity(id, name),
      description(description),
      deadline(deadline),
      priority(priority),
      status(Status::TODO),
      assignedUser(nullptr)
{}

const std::string& Task::getDescription()  const { return description; }
const std::string& Task::getDeadline()     const { return deadline; }
Priority           Task::getPriority()     const { return priority; }
Status             Task::getStatus()       const { return status; }
User*              Task::getAssignedUser() const { return assignedUser; }

void Task::setDescription(const std::string& d) { description = d; }
void Task::setDeadline(const std::string& d)    { deadline = d; }
void Task::setPriority(Priority p)              { priority = p; }
void Task::setStatus(Status s)                  { status = s; }
void Task::assignTo(User* u)                    { assignedUser = u; }

bool Task::isOverdue() const {
    return dateIsBefore(deadline, today()) && status != Status::DONE;
}

bool Task::isDueSoon() const {
    if (status == Status::DONE || isOverdue()) return false;
    time_t now = time(nullptr);
    now += 3 * 24 * 3600;
    tm* t = localtime(&now);
    std::ostringstream oss;
    oss << (1900 + t->tm_year) << "-"
        << std::setw(2) << std::setfill('0') << (1 + t->tm_mon) << "-"
        << std::setw(2) << std::setfill('0') << t->tm_mday;
    std::string limit = oss.str();
    return !dateIsBefore(limit, deadline) && !dateIsBefore(deadline, today());
}

void Task::display() const {
    std::string flag = "";
    if (isOverdue())      flag = " ZAKASNQLA";
    else if (isDueSoon()) flag = " PREDSTOI SKORO";

    std::cout << "  +-- Zadacha #" << id << ": " << name << flag << "\n"
              << "  |  Opisanie : " << description << "\n"
              << "  |  Srok     : " << deadline << "\n"
              << "  |  Prioritet: " << priorityToString(priority) << "\n"
              << "  |  Status   : " << statusToString(status) << "\n";
    if (assignedUser)
        std::cout << "  +  Otgovornik: " << assignedUser->getName()
                  << " <" << assignedUser->getEmail() << ">\n";
    else
        std::cout << "  +  Otgovornik: -\n";
}
