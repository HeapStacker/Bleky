#pragma once
#include <functional>
#include <vector>

namespace bleky {

    class EventBus;

    class IEventChannel {
    public:
        virtual void process() = 0;
        virtual ~IEventChannel() = default;
    };

    template <typename T>
    class EventChannel : public IEventChannel {

        std::vector<T> currentBuffer, nextBuffer;
        std::vector<std::function<void(const T &)>> subscribers;

        /// Swap next→current and dispatch. Events emitted during handlers stay in nextBuffer for a later resolve.
        void process() override {
            currentBuffer.swap(nextBuffer);
            nextBuffer.clear();

            for (const T &event : currentBuffer) {
                for (auto &fn : subscribers) fn(event);
            }

            currentBuffer.clear();
        }

        friend class ::bleky::EventBus;
    };
} // namespace bleky
