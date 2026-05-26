#include "BaseEntity.h"

BaseEntity::BaseEntity(int id, const std::string& name)
    : id(id), name(name) {}

int BaseEntity::getId() const { return id; }
const std::string& BaseEntity::getName() const { return name; }
void BaseEntity::setName(const std::string& n) { name = n; }
