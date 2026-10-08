#include "bleky/ItemManager.hpp"

#include "bleky/Resource.hpp"

namespace bleky {
    Resource::Resource(const std::string &name, ItemManager<Resource> *manager) : Item<Resource>(name, manager) {}

    Resource::Resource(const Resource &other) : Item<Resource>(other.name, other.manager) {}
} // namespace bleky