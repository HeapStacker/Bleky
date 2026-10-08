#include "bleky/ItemBase.hpp"

namespace bleky {
    std::string ItemBase::GenerateName(const std::string &fromOldName) {
        size_t pos = fromOldName.size();
        while (pos > 0 && std::isdigit(static_cast<unsigned char>(fromOldName[pos - 1]))) --pos;
        std::string text = fromOldName.substr(0, pos);
        int value = (pos < fromOldName.size()) ? std::stoi(fromOldName.substr(pos)) : 0;
        return text + std::to_string(++value);
    }
} // namespace bleky
