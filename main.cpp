#include <iostream>
#include "TaskManager.h"
#include "Sorter.h"

int main() {
    std::cout << "=== MENIDZHUR NA ZADACHI - VERSIQ 2 (pulna) ===\n\n";

    TaskManager manager;

    // Потребители
    User* ana  = manager.addUser("Ana Ivanova",  "ana@example.com");
    User* ivan = manager.addUser("Ivan Petrov",  "ivan@example.com");
    std::cout << "\n";

    // 1. Управление на проекти
    manager.createProject("Uebsajt",             "Korporativen sajt");
    manager.createProject("Mobilno prilozhenie", "Android i iOS");
    manager.editProject(1, "Uebsajt v2", "Redizajn na korporativen sajt");
    std::cout << "\n";

    // 2. Добавяне на задачи
    Task* t1 = manager.addTask(1, "Dizajn",      "Figma maketi",      "2024-01-10", Priority::HIGH);
    Task* t2 = manager.addTask(1, "Backend API", "REST endpoints",    "2025-12-31", Priority::MEDIUM);
    Task* t3 = manager.addTask(2, "Login ekran", "UI + auth",         "2025-06-30", Priority::HIGH);
    Task* t4 = manager.addTask(2, "Push notif.", "Firebase setup",    "2025-07-15", Priority::LOW);
    std::cout << "\n";

    // 9. Назначаване на потребители
    manager.assignUser(1, t1->getId(), ana->getId());
    manager.assignUser(1, t2->getId(), ivan->getId());
    std::cout << "\n";

    // 3. Промяна на статус
    manager.changeStatus(1, t1->getId(), Status::IN_PROGRESS);
    manager.changeStatus(2, t3->getId(), Status::DONE);
    std::cout << "\n";

    // 2. Редактиране и изтриване
    manager.editTask(1, t2->getId(), "Backend API v2", "GraphQL + REST", "2025-11-30", Priority::HIGH);
    manager.deleteTask(2, t4->getId());
    std::cout << "\n";

    // 5. Филтриране
    manager.filterByStatus(1, Status::IN_PROGRESS);
    manager.filterByPriority(1, Priority::HIGH);
    std::cout << "\n";

    // 6. Сортиране
    DeadlineSorter bySroc;
    PrioritySorter byPriority;

    std::cout << "\n>> Sortirane po kraen srok:\n";
    manager.displaySorted(1, bySroc);

    std::cout << "\n>> Sortirane po prioritet:\n";
    manager.displaySorted(1, byPriority);

    // 7. Крайни срокове
    manager.checkDeadlines();

    // 8. История
    manager.displayHistory();

    // Показване на всичко
    manager.displayAll();

    std::cout << "\n=== Kraj ===\n";
    return 0;
}
