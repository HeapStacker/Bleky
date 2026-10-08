#pragma once
#include <typeindex>
#include <unordered_map>

#include "bleky/Resource.hpp"
#include "vault/Container.hpp"

namespace bleky {
    class ResourceManager {

        std::unordered_map<std::type_index, vault::Container<ItemManager<Resource>>> resources;

        template <typename Type>
        ItemManager<Resource> &resourceManager() {
            static_assert(std::is_base_of<Resource, Type>::value,
                          "Type must derive from Resource.");

            if (!resources.contains(typeid(Type))) resources[typeid(Type)].create();

            return resources[typeid(Type)].get();
        }

    public:
        template <typename Type, typename... Args>
        Type &fetchResource(const std::string &name, Args &&...args) {
            auto &manager = resourceManager<Type>();

            if (manager.has(name)) return static_cast<Type &>(manager.get(name));

            vault::Container<Resource> resource;
            resource.polyCreate<Type>(name, std::forward<Args>(args)...);

            return static_cast<Type &>(manager.add(std::move(resource)));
        }
    };
} // namespace bleky