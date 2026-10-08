#pragma once

#include "vault/Container.hpp"

namespace bleky {
    template <typename Base, typename Derived>
    class Cloneable : public Base {
    public:
        using Base::Base;

        vault::Container<Base> clone() const override {
            vault::Container<Base> item;
            item.template polyCopyFrom<Derived>(*this);

            return item;
        }
    };
} // namespace bleky