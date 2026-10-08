#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "bleky/Concepts.hpp"

#include "Exporter.hpp"
#include "logger/Logger.hpp"
#include "vault/Container.hpp"

namespace bleky {

    template <typename Type>
    class Item;

    template <typename ItemType>
    class EXPORTED ItemManager {

        std::unordered_map<std::string, vault::Container<ItemType>> items;
        std::unordered_set<std::string> itemsToRemove;

    protected:
        bool rename(const std::string &name, const std::string &newName) {
            if (name == newName) return true;

            auto it = items.find(name);
            if (it == items.end()) {
                LOGGER_LOG_INFO("Item with the current name: " + name + " not found.");
                return false;
            }

            if (has(newName)) {
                LOGGER_LOG_INFO("An item with the same name (newName) already exists in the repository.");
                return false;
            }

            auto node = items.extract(it);
            node.key() = newName;
            node.mapped()->name = newName;
            items.insert(std::move(node));

            return true;
        }

    public:
        ItemManager() {
            static_assert(std::is_base_of<Item<ItemType>, ItemType>::value,
                          "ItemType must derive from Item.");
        };

        ItemManager(const ItemManager &other) {
            items = other.items;
            itemsToRemove = other.itemsToRemove;
            for (auto &[_, item] : items) item.get().manager = this;
        }

        ItemManager(ItemManager<ItemType> &&other) noexcept {
            items = std::move(other.items);
            itemsToRemove = std::move(other.itemsToRemove);
            other.items.clear();
            other.itemsToRemove.clear();

            for (auto &[_, item] : items) item.get().manager = this;
        }

        ItemManager operator=(const ItemManager<ItemType> &other) {
            if (this == &other) return *this;
            items = other.items;
            itemsToRemove = other.itemsToRemove;
            for (auto &[_, item] : items) item.get().manager = this;
            return *this;
        }

        ItemManager operator=(ItemManager<ItemType> &&other) noexcept {
            if (this == &other) return *this;
            items = std::move(other.items);
            itemsToRemove = std::move(other.itemsToRemove);
            other.items.clear();
            other.itemsToRemove.clear();
            for (auto &[_, item] : items) item.get().manager = this;
            return *this;
        }

        template <typename... Args>
        ItemType &add(std::string baseName, Args &&...args) {
            while (has(baseName)) baseName = ItemType::GenerateName(baseName);

            vault::Container<ItemType> item;
            item.create(baseName, std::forward<Args>(args)...);
            item.get().manager = this;

            return items.emplace(baseName, std::move(item)).first->second.get();
        }

        // WARNING: use only when you want to copy non polymorphic items!
        ItemType &add(const ItemType &item) {
            vault::Container<ItemType> item_;
            item_.assign(item);

            return add(std::move(item_));
        }

        ItemType &add(vault::Container<ItemType> &&item) {
            ItemType &itemRef = item.get();
            if (itemRef.getManager()) LOGGER_THROW_ERROR("Can't add already attached item to a manager.");

            while (has(itemRef.name)) itemRef.name = ItemType::GenerateName(itemRef.name);
            itemRef.manager = this;

            return items.emplace(itemRef.name, std::move(item)).first->second.get();
        }

        bool has(const std::string &name = "") const {
            auto it = items.find(name);
            return it != items.end();
        }

        bool empty() const {
            return items.empty();
        }

        bool has(const ItemType &item) const {
            return has(item.name) && get(item.name) == &item;
        }

        ItemType &get(const std::string &name) {
            if (!has(name)) LOGGER_THROW_ERROR("Item with name " + name + " not found.");
            return items[name].get();
        }

        std::vector<ItemType *> getAll() const {
            std::vector<ItemType *> temp;
            for (auto &[_, item] : items) temp.push_back(&item.get());
            return temp;
        }

        bool remove(const std::string &name) {
            if (has(name)) get(name).Item<ItemType>::destroy();
        }

        void remove(const ItemType &item) {
            if (has(item)) item.Item<ItemType>::destroy();
        }

        void removeAll() {
            for (const auto &[_, item] : items) item.Item<ItemType>::destroy();
        }

        // Only call this from designated places!
        void clearRemoved() {
            for (const auto &name : itemsToRemove) items.erase(name);
            itemsToRemove.clear();
        }

        friend class ::bleky::Item<ItemType>;
    };
}; // namespace bleky
