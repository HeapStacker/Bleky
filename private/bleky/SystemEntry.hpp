#pragma once
#include "bleky/System.hpp"

namespace bleky {
    class SystemGroup;

    class SystemEntry {
        BlekySystem fn{nullptr};

    public:
        explicit SystemEntry(BlekySystem system)
            : fn(system) {}

        EntityMap activeEntities{};
        EntityMap inactiveEntities{};

        friend class ::bleky::SystemGroup;
    };
} // namespace bleky