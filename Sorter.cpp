#include "Sorter.h"
#include "Utils.h"

void Sorter::sort(std::vector<Task*>& tasks) const {
    std::sort(tasks.begin(), tasks.end(),
              [this](const Task* a, const Task* b) {
                  return this->compare(a, b);
              });
}

bool DeadlineSorter::compare(const Task* a, const Task* b) const {
    return dateIsBefore(a->getDeadline(), b->getDeadline());
}

bool PrioritySorter::compare(const Task* a, const Task* b) const {
    return priorityValue(a->getPriority()) > priorityValue(b->getPriority());
}
