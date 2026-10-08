#pragma once
#include "Exporter.hpp"
#include "bleky/Component.hpp"


namespace bleky::cmp {
    class ItemBase;

    template <typename ItemType>
    class Item;

    class EXPORTED Ref : public BLEKY_COMPONENT(Ref) {
        ItemBase *to_{nullptr};

    public:
        Ref();

        template <typename ItemType>
        void to(Item<ItemType> &item);

        // WARNING: carefuly specify the correct type!
        template <typename ItemType>
        ItemType &get();
    };
} // namespace bleky::cmp