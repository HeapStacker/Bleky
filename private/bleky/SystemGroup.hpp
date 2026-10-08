#pragma once
#include <cstdint>
#include <list>
#include <string>
#include <typeindex>
#include <unordered_set>
#include <vector>

#include "bleky/Item.hpp"
#include "bleky/SystemEntry.hpp"

#include "bleky/System.hpp"

namespace bleky {

    class Component;
    class SystemManager;

    class SystemGroup : public Item<SystemGroup> {
        std::list<SystemEntry> systems;
        std::unordered_set<BlekySystem> systemsToRemove;
        std::list<SystemEntry>::iterator anchor = systems.begin();

        // helpers...
        void addActiveEntity(SystemEntry *entry, Entity *entity, std::type_index componentGroup);
        void addInactiveEntity(SystemEntry *entry, Entity *entity, std::type_index componentGroup);

        std::string name;
        size_t priority;

    public:
        SystemGroup(const std::string &name, size_t priority);
        SystemGroup(const SystemGroup &other);

        bool setAnchor(SystemEntry &entry);
        bool setAnchor(size_t index = 0);
        void setAnchorOnLast();
        bool setAnchorOnNth(BlekySystem system, uint8_t n = 0);
        bool setAnchorOnLast(BlekySystem system);

        SystemEntry &addSystem(BlekySystem system, bool afterAnchor = true);
        SystemEntry &addSystem(std::vector<BlekySystem> systems, bool afterAnchor = true);

        void removeSystem(BlekySystem system);

        // Getters...
        std::string getName() const;
        size_t getPriority() const;
        SystemEntry *getAnchor() const;
        bool hasSystem(BlekySystem system) const;
        size_t getCountOf(BlekySystem systems) const;
        std::vector<SystemEntry *> getSystems(BlekySystem systems);

        // Execution...
        void executeAll();

        // adding & removing entities to/from systems...
        void addEntityTo(BlekySystem system, Entity *entity, std::type_index componentGroup);
        void removeEntityFrom(BlekySystem system, Entity *entity);

        // Only call this from designated places!
        void clearRemoved();
    };
} // namespace bleky
