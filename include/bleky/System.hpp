#pragma once
#include <typeindex>
#include <unordered_map>
#include <vector>

// use this macro within a system to get the entity group!
// example:
// void mySystem(EntityMap &funnyMap) {
//     for (auto entity : BLEKY_ENTITIES(funnyMap,MyComponent)) {
//         // do something with the entity
//     }
//
// }
#define BLEKY_ENTITIES(mapName, ComponentType) mapName[typeid(ComponentType)]

namespace bleky {
    class Entity;

    using EntityGroup = std::vector<Entity *>;
    using EntityMap = std::unordered_map<std::type_index, EntityGroup>;
    using BlekySystem = void (*)(EntityMap &);

} // namespace bleky