#pragma once
#include <cctype>

// package headers...
#include "Exporter.hpp"
#include "vault/Destroyable.hpp"
#include "vault/View.hpp"

// core headers...
#include "bleky/Concepts.hpp"
#include "bleky/ItemBase.hpp"
#include "bleky/ItemManager.hpp"

namespace bleky {

    template <typename Type>
    class EXPORTED Item : public ItemBase, public vault::Destroyable {
    protected:
        void onDestroy() override {
            if (manager) manager->itemsToRemove.insert(name);
        }

        vault::View<ItemManager<Type>, false, false> manager;

    public:
        Item<Type>() {}

        Item<Type>(const std::string &name) : ItemBase(name) {}

        Item<Type>(const std::string &name, ItemManager<Type> *manager) : ItemBase(name) {
            manager.assign(manager);
        }

        // getters...
        ItemManager<Type> *getManager() const {
            return manager.get();
        }

        // methods...
        bool rename(const std::string &newName) {
            auto man = manager.get();
            if (man) return man->rename(this->name, newName);

            name = newName;
            return true;
        }

        friend class ::bleky::ItemManager<Type>;
    };

} // namespace bleky
