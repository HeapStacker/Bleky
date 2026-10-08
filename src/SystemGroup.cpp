#include <iterator>

#include "bleky/SystemEntry.hpp"
#include "bleky/SystemGroup.hpp"

#include "bleky/Entity.hpp"

namespace bleky {

    void SystemGroup::addActiveEntity(SystemEntry *entry, Entity *entity, std::type_index componentGroup) {
        auto &actives = entry->activeEntities[componentGroup];
        if (entry->inactiveEntities.contains(componentGroup)) std::erase(entry->inactiveEntities[componentGroup], entity);
        if (std::find(actives.begin(), actives.end(), entity) == actives.end()) actives.push_back(entity);
    }

    void SystemGroup::addInactiveEntity(SystemEntry *entry, Entity *entity, std::type_index componentGroup) {
        auto &inactives = entry->inactiveEntities[componentGroup];
        if (entry->activeEntities.contains(componentGroup)) std::erase(entry->activeEntities[componentGroup], entity);
        if (std::find(inactives.begin(), inactives.end(), entity) == inactives.end()) inactives.push_back(entity);
    }

    void SystemGroup::addEntityTo(BlekySystem system, Entity *entity, std::type_index componentGroup) {

        if (!hasSystem(system)) addSystem(system);

        if (entity->active()) {
            for (auto *entry : getSystems(system)) addActiveEntity(entry, entity, componentGroup);
        } else {
            for (auto *entry : getSystems(system)) addInactiveEntity(entry, entity, componentGroup);
        }
    }

    void SystemGroup::removeEntityFrom(BlekySystem system, Entity *entity) {
        if (!hasSystem(system)) return;

        for (auto *entry : getSystems(system)) {
            for (auto &[_, entities] : entry->activeEntities) std::erase(entities, entity);
            for (auto &[_, entities] : entry->inactiveEntities) std::erase(entities, entity);
        }
    }

    void SystemGroup::clearRemoved() {
        for (auto it = systems.begin(); it != systems.end();) {
            if (!systemsToRemove.contains(it->fn)) {
                ++it;
                continue;
            }

            if (it == anchor) anchor = std::next(it);

            it = systems.erase(it);
        }

        systemsToRemove.clear();
    }

    SystemGroup::SystemGroup(const std::string &name, size_t priority) : name(name), priority(priority) {}

    SystemGroup::SystemGroup(const SystemGroup &other) : name(other.name), priority(other.priority), systems(other.systems), systemsToRemove(other.systemsToRemove) {
        auto it = systems.begin();

        for (auto otherIt = other.systems.begin();
             otherIt != other.anchor;
             ++otherIt, ++it);

        anchor = it;
    }

    bool SystemGroup::setAnchor(SystemEntry &entry) {
        for (auto it = systems.begin(); it != systems.end(); it++) {
            if (&*it == &entry) {
                anchor = it;
                return true;
            }
        }
        return false;
    }

    bool SystemGroup::setAnchor(size_t index) {
        if (index >= systems.size()) return false;
        else {
            anchor = systems.begin();
            std::advance(anchor, index);
            return true;
        }
    }

    void SystemGroup::setAnchorOnLast() {
        anchor = systems.empty() ? systems.end() : --systems.end();
    }

    bool SystemGroup::setAnchorOnNth(BlekySystem system, uint8_t n) {
        uint8_t found = 0;
        for (auto it = systems.begin(); it != systems.end(); it++) {
            if (it->fn == system) {
                if (found == n) {
                    anchor = it;
                    return true;
                }
                ++found;
            }
        }
        return false;
    }

    bool SystemGroup::setAnchorOnLast(BlekySystem system) {
        if (systems.empty()) return false;

        for (auto it = systems.rbegin(); it != systems.rend(); ++it) {
            if (it->fn == system) {
                anchor = std::prev(it.base());
                return true;
            }
        }

        return false;
    }

    SystemEntry &SystemGroup::addSystem(BlekySystem system, bool afterAnchor) {
        anchor = systems.insert(afterAnchor ? std::next(anchor) : anchor, SystemEntry(system));
        return *anchor;
    }

    SystemEntry &SystemGroup::addSystem(std::vector<BlekySystem> systems, bool afterAnchor) {
        for (auto &s : systems) addSystem(s, afterAnchor);
        return *anchor;
    }

    void SystemGroup::removeSystem(BlekySystem system) {
        systemsToRemove.insert(system);
    }

    std::string SystemGroup::getName() const {
        return name;
    }

    size_t SystemGroup::getPriority() const {
        return priority;
    }

    SystemEntry *SystemGroup::getAnchor() const {
        return anchor == systems.end() ? nullptr : &*anchor;
    }

    bool SystemGroup::hasSystem(BlekySystem system) const {
        for (const auto &e : systems)
            if (e.fn == system) return true;
        return false;
    }

    size_t SystemGroup::getCountOf(BlekySystem systems) const {
        size_t count = 0;
        for (auto &system : this->systems)
            if (system.fn == systems) count++;
        return count;
    }

    std::vector<SystemEntry *> SystemGroup::getSystems(BlekySystem systems) {
        std::vector<SystemEntry *> entries;
        for (auto &system : this->systems) {
            if (system.fn == systems) entries.push_back(&system);
        }
        return entries;
    }

    void SystemGroup::executeAll() {
        for (auto it = systems.begin(); it != systems.end(); it++) it->fn(it->activeEntities);
    }

} // namespace bleky