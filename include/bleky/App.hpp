#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "bleky/Item.hpp"
#include "bleky/ResourceManager.hpp"

#include "Exporter.hpp"
#include "bleky/EventBus.hpp"
#include "vault/Container.hpp"


namespace bleky {
    class Scene;

    class EXPORTED App final : public Item<App>, public ResourceManager, public EventBus {
        uint64_t frameCount{0};
        static inline bool Created{false};

        App() = default;
        App(const App &) = delete;
        App(App &&) = delete;
        App &operator=(const App &) = delete;
        App &operator=(App &&) = delete;

        std::unordered_map<std::string, vault::Container<Scene>> scenes;
        std::unordered_set<std::string> scenesToRemove;

        static inline Scene *ActiveScene{nullptr};
        static inline Scene *NextActiveScene{nullptr};

    public:
        static App &Get();

        float fps{60.f};
        float deltaTime{0.f};

        const uint64_t &getFrameCount() {
            return frameCount;
        }

        Scene &add(std::string name);
        bool has(const std::string &name) const;
        Scene &get(const std::string &name);

        void run();

        friend class ::bleky::Scene;
    };
} // namespace bleky
