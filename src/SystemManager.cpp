#include "bleky/SystemManager.hpp"

#include "logger/Logger.hpp"

namespace bleky {

    void SystemManager::copy(SystemManager &other) {
        groups = other.groups;
        for (auto &group : groups) {
            groupNames[group.getName()] = &group;
            groupOrder[-static_cast<int>(group.getPriority())] = &group;
        }
    }

    void SystemManager::clear() {
        groups.clear();
        groupNames.clear();
        groupOrder.clear();
    }

    SystemManager::SystemManager(SystemManager &other) {
        copy(other);
    }

    SystemManager::SystemManager(SystemManager &&other) {
        copy(other);
        other.clear();
    }

    SystemManager &SystemManager::operator=(SystemManager &other) {
        if (this == &other) return *this;
        copy(other);
        return *this;
    }

    SystemManager &SystemManager::operator=(SystemManager &&other) {
        if (this == &other) return *this;
        copy(other);
        other.clear();
        return *this;
    }

    SystemGroup &SystemManager::addGroup(const std::string &name, size_t priority) {
        if (groupNames.find(name) != groupNames.end()) LOGGER_THROW_ERROR("SystemGroup " + name + " already exists");
        if (groupOrder.find(priority) != groupOrder.end()) LOGGER_THROW_ERROR("SystemGroup priority " + std::to_string(priority) + " already exists");

        groups.push_back(SystemGroup(name, priority));
        SystemGroup &group = groups.back();

        groupNames[name] = &group;
        groupOrder[-static_cast<int>(priority)] = &group;

        return group;
    }

    bool SystemManager::hasGroup(const std::string &name) {
        return groupNames.find(name) != groupNames.end();
    }

    SystemGroup &SystemManager::getGroup(const std::string &name) {
        if (groupNames.find(name) == groupNames.end()) LOGGER_THROW_ERROR("SystemGroup " + name + " not found");
        return *groupNames[name];
    }

    void SystemManager::executeAll() {
        for (auto &[_, group] : groupOrder) {
            group->executeAll();
        }
    }

    void SystemManager::clearRemoved() {
        for (auto &group : groups) {
            group.clearRemoved();
        }
    }
} // namespace bleky