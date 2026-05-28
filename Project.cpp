#include "Project.h"
#include <iostream>

Project::Project(int id, const std::string& name, const std::string& desc)
    : BaseEntity(id, name), description(desc) {}

const std::string& Project::getDescription() const { return description; }
void Project::setDescription(const std::string& d) { description = d; }

Task* Project::addTask(int id, const std::string& taskName, const std::string& desc,
                       const std::string& deadline, Priority priority) {
    tasks.push_back(std::make_unique<Task>(id, taskName, desc, deadline, priority));
    return tasks.back().get();
}

bool Project::removeTask(int taskId) {
    for (auto it = tasks.begin(); it != tasks.end(); ++it) {
        if ((*it)->getId() == taskId) {
            tasks.erase(it);
            return true;
        }
    }
    return false;
}

Task* Project::findTask(int taskId) {
    for (auto& t : tasks)
        if (t->getId() == taskId) return t.get();
    return nullptr;
}

std::vector<Task*> Project::getAllTasks() const {
    std::vector<Task*> result;
    for (const auto& t : tasks) result.push_back(t.get());
    return result;
}

std::vector<Task*> Project::filterByStatus(Status s) const {
    std::vector<Task*> result;
    for (const auto& t : tasks)
        if (t->getStatus() == s) result.push_back(t.get());
    return result;
}

std::vector<Task*> Project::filterByPriority(Priority p) const {
    std::vector<Task*> result;
    for (const auto& t : tasks)
        if (t->getPriority() == p) result.push_back(t.get());
    return result;
}

void Project::display() const {
    std::cout << "\n+======================================+\n"
              << "  Проект #" << id << ": " << name << "\n"
              << "  " << description << "\n"
              << "  Задачи: " << tasks.size() << "\n"
              << "+======================================+\n";
    if (tasks.empty())
        std::cout << "  (няма задачи)\n";
    else
        for (const auto& t : tasks)
            t->display();
}
