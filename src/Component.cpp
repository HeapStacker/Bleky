#include "bleky/Item.hpp"

#include "bleky/Component.hpp"
#include "bleky/Entity.hpp"
#include "bleky/Scene.hpp"

namespace bleky {

    void Component::onDestroy() {
        if (!entity) return;

        for (auto system : registeredSystems) {
            entity->linkedTo[system]--;

            if (!entity->linkedTo[system]) entity->getScene().getGroup(systemGroup).removeEntityFrom(system, entity);
        }
    }

    void Component::linkToSystem(std::type_index componentType) {
        if (!entity) return;

        for (auto system : registeredSystems) {
            entity->getScene().getGroup(systemGroup).addEntityTo(system, entity, componentType);
        }
    }

    void Component::destroy() {
        LOGGER_LOG_WARNING("You should not call destroy() on a Component. Use Entity::removeComponent() instead.");
    }

    Component::Component(const std::string &name, Entity *entity) : Item<Component>(name), entity(entity) {}

} // namespace bleky