#pragma once
#include <cstddef>
#include <string>

#include "bleky/ResourceManager.hpp"
#include "bleky/SystemManager.hpp"

#include "Exporter.hpp"
#include "bleky/Entity.hpp"
#include "bleky/EventBus.hpp"


namespace bleky {

    class App;

    class EXPORTED Scene final : public Entity, public SystemManager, public ResourceManager, public EventBus {

        void onDestroy() override;

        Scene(const std::string &name);

    public:
        void activate();

        Entity &add(std::string name);
        Entity &add(const Entity &child);

        void run();

        friend class ::bleky::App;
    };
} // namespace bleky
