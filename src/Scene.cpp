#include "bleky/Scene.hpp"
#include "bleky/App.hpp"

namespace bleky {

    void Scene::onDestroy() {
        Entity::onDestroy();

        if (App::ActiveScene == this) App::ActiveScene = nullptr;
        if (App::NextActiveScene == this) App::NextActiveScene = nullptr;

        App::Get().scenesToRemove.insert(name);
    }

    Scene::Scene(const std::string &name) : Entity(name, nullptr) {
        isScene = true;

        if (!App::ActiveScene) activate();
    }

    void Scene::activate() {
        for (auto &[_, scene] : App::Get().scenes) scene.get().Entity::activate(false);
        App::NextActiveScene = this;

        if (!App::ActiveScene) App::ActiveScene = this;
    }

    Entity &Scene::add(std::string name) {
        auto &entity = Entity::add(name);
        entity.activate(true);
        return entity;
    }

    Entity &Scene::add(const Entity &child) {
        auto &entity = Entity::add(child);
        entity.activate(true);
        return entity;
    }

    void Scene::run() {
        // executing systems...
        resolve();
        SystemManager::executeAll();

        // clearing removed items...
        SystemManager::clearRemoved();
        Entity::clearRemoved();
    }
} // namespace bleky
