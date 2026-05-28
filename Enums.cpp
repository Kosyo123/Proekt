#include "Enums.h"

std::string priorityToString(Priority p) {
    switch (p) {
        case Priority::LOW:    return "Ниско";
        case Priority::MEDIUM: return "Средно";
        case Priority::HIGH:   return "Високо";
    }
    return "Непознато";
}

std::string statusToString(Status s) {
    switch (s) {
        case Status::TODO:        return "За правене";
        case Status::IN_PROGRESS: return "Във момента се прави";
        case Status::DONE:        return "Направено";
    }
    return "Непознато";
}

int priorityValue(Priority p) {
    switch (p) {
        case Priority::LOW:    return 0;
        case Priority::MEDIUM: return 1;
        case Priority::HIGH:   return 2;
    }
    return 0;
}
