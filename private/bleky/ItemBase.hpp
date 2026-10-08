#pragma once
#include <cctype>
#include <string>

namespace bleky {

    class ItemBase {
    protected:
        std::string name;

    public:
        ItemBase() = default;
        ItemBase(const std::string &name) : name(name) {}

        // utility...
        static std::string GenerateName(const std::string &fromOldName);

        // getters...
        const std::string &getName() const { return name; }
    };
} // namespace bleky