#include "bleky/Entity.hpp"
#include "bleky/Scene.hpp"

namespace bleky {

    void Entity::copy(const Entity &other) {
        name = other.name;
        manager = nullptr;
        linkedTo = other.linkedTo;

        for (auto &[type, componentManager] : other.components) {
            vault::Container<ItemManager<Component>> cMan;
            cMan.copyFrom(*componentManager);

            components[type] = std::move(cMan);
        }

        for (auto child : other.getAll()) {
            vault::Container<Entity> entity;
            entity.copyFrom(*child);

            add(std::move(entity)).copy(*child);
        }
    }

    void Entity::onDestroy() {
        removeAllComponents();
        removeAll();
    }

    Entity::Entity(const std::string &name, ItemManager<Entity> *manager) : Item(name, manager) {}

    Entity::Entity(const Entity &other) : Item<Entity>(other) {
        copy(other);
    }

    Entity::Entity(Entity &&other) : Item<Entity>(std::move(other)) {
        linkedTo = other.linkedTo;
        components = other.components;
        active_ = false;
    }

    Entity &Entity::operator=(const Entity &other) {
        if (this == &other) return *this;
        copy(other);
        return *this;
    }

    Entity &Entity::operator=(Entity &&other) {
        if (this == &other) return *this;
        copy(other);
        other.destroy();
        return *this;
    }

    void Entity::clearRemoved() {
        for (auto *child : getAll()) child->clearRemoved();

        ItemManager<Entity>::clearRemoved();
        for (auto &[type, manager] : components) manager.get().clearRemoved();
    }

    Scene &Entity::getScene() {
        if (isScene) return *static_cast<Scene *>(this);
        Entity *parent_ = this->parent;
        while (parent_) {
            if (parent_->isScene) return *static_cast<Scene *>(parent_);
            parent_ = parent_->parent;
        }

        LOGGER_THROW_ERROR("Entity is not attached to the scene.");
        exit(0);
    }

    bool Entity::active() const {
        return active_;
    }

    void Entity::activate(bool activate) {
        auto scene = getScene();
        active_ = activate;

        for (auto &[type, manager] : components) {
            for (auto component : manager.get().getAll()) {
                component->linkToSystem(type);
            }
        }
    }

    Entity &Entity::add(std::string name) {

        auto &entity = ItemManager<Entity>::add(name, this);
        entity.parent = this;

        return entity;
    }

    Entity &Entity::add(const Entity &child) {
        auto &entity = ItemManager<Entity>::add(child);
        entity.parent = this;

        return entity;
    }

    Entity &Entity::add(vault::Container<Entity> &&child) {
        auto &entity = ItemManager<Entity>::add(std::move(child));
        entity.parent = this;

        return entity;
    }

} // namespace bleky
