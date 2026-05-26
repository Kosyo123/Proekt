#include "User.h"

User::User(int id, const std::string& name, const std::string& email)
    : id(id), name(name), email(email) {}

int                User::getId()    const { return id; }
const std::string& User::getName()  const { return name; }
const std::string& User::getEmail() const { return email; }
