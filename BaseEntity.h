#pragma once
#include <string>

// Абстрактен базов клас — осигурява абстракция и полиморфизъм
class BaseEntity {
protected:
    int         id;
    std::string name;

public:
    BaseEntity(int id, const std::string& name);
    virtual ~BaseEntity() = default;

    // Чисто виртуален метод — абстракция
    virtual void display() const = 0;

    int                getId()   const;
    const std::string& getName() const;
    void               setName(const std::string& n);
};
