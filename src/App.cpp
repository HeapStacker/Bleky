#include <thread>

#include "bleky/App.hpp"
#include "bleky/Entity.hpp"
#include "bleky/Scene.hpp"

namespace bleky {

    namespace {
        void initConsoleUtf8() {
#ifdef _WIN32
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
#endif
        }
    } // namespace

    App &App::Get() {
        static std::unique_ptr<App> instance;

        if (!Created) {
            instance = std::unique_ptr<App>(new App());
            Created = true;
        }

        return *instance.get();
    }

    Scene &App::add(std::string name) {
        if (has(name)) LOGGER_THROW_ERROR("Scene with name " + name + " already exists.");
        return scenes.emplace(name, vault::Container<Scene>()).first->second.get();
    }

    bool App::has(const std::string &name) const {
        return scenes.contains(name);
    }

    Scene &App::get(const std::string &name) {
        if (!has(name)) LOGGER_THROW_ERROR("Scene with name " + name + " not found.");
        return scenes[name].get();
    }

    void App::run() {
        initConsoleUtf8();

        std::chrono::steady_clock::time_point now, lastFrame, fullTime;
        float frameDuration{0.f};

        while (isAlive()) {
            now = std::chrono::steady_clock::now();
            deltaTime = std::chrono::duration<float>(now - lastFrame).count();
            lastFrame = now;

            frameCount++;

            resolve();
            if (App::ActiveScene) App::ActiveScene->run();
            App::ActiveScene = App::NextActiveScene;

            for (auto &name : scenesToRemove) scenes.erase(name);
            scenesToRemove.clear();

            frameDuration = 1.0f / fps;
            fullTime = lastFrame + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                                       std::chrono::duration<float>(frameDuration));
            now = std::chrono::steady_clock::now();
            if (now < fullTime) {
                std::this_thread::sleep_until(fullTime);
            }
        }
    }
} // namespace bleky
