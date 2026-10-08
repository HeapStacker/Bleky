#pragma once
#include <vector>

// package headers...
#include "vault/Container.hpp"
#include "vault/View.hpp"

// core headers...
#include "bleky/SystemManager.hpp"

// public headers...
#include "Exporter.hpp"
#include "bleky/System.hpp"

// use this macro to define a component
// example:
// class MyComponent : public BLEKY_COMPONENT(MyComponent) {
//     public:
// ...
#define BLEKY_COMPONENT(ConcreteType) Cloneable<Component, ConcreteType>

namespace bleky {

    class Entity;

    template <typename Type>
    class Item;

    class EXPORTED Component : public Item<Component>, public vault::ICloneable<Component> {
    protected:
        bool vital{false};

        std::string systemGroup = "clientGroup";
        size_t systemGroupPriority = BLEKY_MAX_PRIORITY / 2;
        std::vector<BlekySystem> registeredSystems;

        vault::View<Entity, false, true> entity;

        void onDestroy() override;

    public:
        Component() = default;
        explicit Component(const std::string &name, Entity *entity = nullptr);

        void linkToSystem(std::type_index componentType);

        void destroy();

        virtual ~Component() = default;

        friend class ::bleky::Entity;
    };

} // namespace bleky
