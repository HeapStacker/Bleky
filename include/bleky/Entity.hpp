#pragma once
#include <cstdlib>
#include <typeindex>
#include <unordered_map>
#include <vector>

// package headers...
#include "Exporter.hpp"
#include "logger/Logger.hpp"
#include "vault/Container.hpp"

// core headers...
#include "bleky/Item.hpp"
#include "bleky/ItemManager.hpp"

// public headers...
#include "bleky/Component.hpp"
#include "bleky/System.hpp"

namespace bleky {

    class Scene;
    class SystemGroup;

    class EXPORTED Entity : public Item<Entity>, public ItemManager<Entity> {
        std::unordered_map<BlekySystem, size_t> linkedTo;
        std::unordered_map<std::type_index, vault::Container<ItemManager<Component>>> components;

        Entity *parent{nullptr};
        bool active_{false};

        template <typename Type>
        ItemManager<Component> &componentManager() {
            static_assert(std::is_base_of<Component, Type>::value,
                          "Type must derive from Component.");

            if (!components.contains(typeid(Type))) components[typeid(Type)].create();

            return components[typeid(Type)].get();
        }

        void copy(const Entity &other);

    protected:
        bool isScene{false};
        void onDestroy() override;

    public:
        Entity() = default;
        Entity(const std::string &name, ItemManager<Entity> *manager);
        Entity(const Entity &other);
        Entity(Entity &&other);

        Entity &operator=(const Entity &other);
        Entity &operator=(Entity &&other);

        Scene &getScene();

        // activation...
        bool active() const;
        void activate(bool activate = true);

        // Entity hyerarchy management...
        Entity &add(std::string name);
        Entity &add(const Entity &child);
        Entity &add(vault::Container<Entity> &&child);
        // there are even more hierarchy management methods but they were not modified in this class - still you should use them (look into ItemManager.hpp for every method)

        // Component management...
        template <typename ComponentType, typename... Args>
        ComponentType &addNamedComponent(const std::string &name, Args &&...args) {
            auto &manager = componentManager<ComponentType>();

            if (manager.has(name)) {
                LOGGER_LOG_INFO("Component with name " + name + " already exists.");
                return static_cast<ComponentType &>(manager.get(name));
            }

            vault::Container<Component> comp;
            comp.polyCreate<ComponentType>(name, this, std::forward<Args>(args)...);

            auto &component = manager.add(std::move(comp));

            for (auto system : component.registeredSystems) linkedTo[system]++;
            component.linkToSystem(typeid(ComponentType));

            return static_cast<ComponentType &>(component);
        }

        template <typename ComponentType, typename... Args>
        ComponentType &addComponent(Args &&...args) {
            return addNamedComponent<ComponentType>("", std::forward<Args>(args)...);
        }

        template <typename ComponentType>
        ComponentType &addComponent(ComponentType &component) {
            vault::Container<Component> comp;
            comp.polyCopyFrom<ComponentType>(component);

            for (auto system : component.registeredSystems) linkedTo[system]++;
            component.linkToSystem(typeid(ComponentType));

            return static_cast<ComponentType &>(componentManager<ComponentType>().add(std::move(comp)));
        }

        template <typename ComponentType>
        bool hasComponent(const std::string &name = "") {
            return componentManager<ComponentType>().has(name);
        }

        template <typename ComponentType>
        ComponentType &getComponent(const std::string &name = "") {
            return static_cast<ComponentType &>(componentManager<ComponentType>().get(name));
        }

        template <typename ComponentType>
        std::vector<ComponentType *> getAllComponents() {
            std::vector<ComponentType *> components;
            for (auto &component : componentManager<ComponentType>().getAll()) components.push_back(&static_cast<ComponentType &>(component));
            return components;
        }

        template <typename ComponentType>
        void removeComponent(const std::string &name = "") {
            componentManager<ComponentType>().remove(name);
        }

        template <typename ComponentType>
        void removeComponent(ComponentType &component) {
            componentManager<ComponentType>().remove(component);
        }

        template <typename ComponentType>
        void removeAllComponentsOfType() {
            componentManager<ComponentType>().removeAll();
        }

        void removeAllComponents() {
            for (auto &[type, manager] : components) manager.get().removeAll();
        }

        // Only call this from designated places!
        void clearRemoved();

        friend class ::bleky::Component;
    };
} // namespace bleky