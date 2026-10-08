#pragma once

#include <algorithm>
#include <typeindex>
#include <utility>

#include "bleky/EventChannel.hpp"

#include "Exporter.hpp"
#include "vault/Container.hpp"

namespace bleky {

    class EXPORTED EventBus {

        std::unordered_map<std::type_index, vault::Container<IEventChannel>> channels;

        template <typename Event>
        EventChannel<Event> *getChannel() {
            std::type_index key(std::type_index(typeid(Event)));

            auto it = channels.find(key);

            if (it == channels.end()) {
                auto ch = std::make_unique<EventChannel<Event>>();
                EventChannel<Event> *ptr = ch.get();
                channels[key] = std::move(ch);
                return ptr;
            }

            return static_cast<EventChannel<Event> *>(it->second.get());
        }

    public:
        template <typename T>
        void emit(const T &event) {
            getChannel<T>()->nextBuffer.push_back(event);
        }

        template <typename T>
        void subscribe(std::function<void(const T &)> fn) {
            getChannel<T>()->subscribers.emplace_back(std::move(fn));
        }

        void resolve() {
            for (auto &[_, channel] : channels) channel.get().process();
        }
    };

} // namespace bleky
