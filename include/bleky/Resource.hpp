#pragma once

// package headers...
#include "vault/Container.hpp"

// core headers...
#include "bleky/Concepts.hpp"
#include "bleky/Item.hpp"

// public headers...
#include "Exporter.hpp"

// use this macro to define a resource
// example:
// class Texture : public BLEKY_RESOURCE(Texture) {
//     public:
// ...
#define BLEKY_RESOURCE(ConcreteType) Cloneable<Resource, ConcreteType>

namespace bleky {

    template <typename ItemType>
    class ItemManager;

    class ResourceManager;

    class EXPORTED Resource : public Item<Resource>, public vault::ICloneable<Resource> {
    protected:
        Resource(const std::string &name, ItemManager<Resource> *manager);
        Resource(const Resource &other);
        virtual ~Resource() = default;

        Resource(Resource &&other) = delete;
        Resource &operator=(const Resource &other) = delete;
        Resource &operator=(Resource &&other) = delete;

        friend class ::bleky::ResourceManager;
    };
} // namespace bleky
