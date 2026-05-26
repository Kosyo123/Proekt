#pragma once
#include "Task.h"
#include <vector>
#include <algorithm>

// Абстрактна стратегия за сортиране — Абстракция + Полиморфизъм
class Sorter {
public:
    virtual ~Sorter() = default;
    virtual bool compare(const Task* a, const Task* b) const = 0;

    void sort(std::vector<Task*>& tasks) const;
};

// Сортиране по краен срок (ascending)
class DeadlineSorter : public Sorter {
public:
    bool compare(const Task* a, const Task* b) const override;
};

// Сортиране по приоритет (HIGH -> LOW)
class PrioritySorter : public Sorter {
public:
    bool compare(const Task* a, const Task* b) const override;
};
