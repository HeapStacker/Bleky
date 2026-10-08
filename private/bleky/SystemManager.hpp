#pragma once
#include <map>
#include <unordered_map>
#include <vector>

#include "bleky/SystemGroup.hpp"

#define BLEKY_MAX_PRIORITY 100

namespace bleky {
    class SystemManager {
        std::vector<SystemGroup> groups;
        std::unordered_map<std::string, SystemGroup *> groupNames;
        std::map<int, SystemGroup *> groupOrder;

        void copy(SystemManager &other);
        void clear();

    public:
        SystemManager() = default;
        SystemManager(SystemManager &other);
        SystemManager(SystemManager &&other);
        SystemManager &operator=(SystemManager &other);
        SystemManager &operator=(SystemManager &&other);

        SystemGroup &addGroup(const std::string &name = "clientGroup", size_t priority = BLEKY_MAX_PRIORITY / 2);
        bool hasGroup(const std::string &name = "clientGroup");
        SystemGroup &getGroup(const std::string &name = "clientGroup");

        void executeAll();
        void clearRemoved();
    };
} // namespace bleky