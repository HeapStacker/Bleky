#include "bleky/components/Ref.hpp"

namespace bleky::cmp {

    template <typename ItemType>
    void Ref::to(Item<ItemType> &item) {
        to_ = &item;
    }

    template <typename ItemType>
    ItemType &Ref::get() {
        return static_cast<ItemType &>(*to_);
    }
} // namespace bleky::cmp