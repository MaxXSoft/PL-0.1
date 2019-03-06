#include <define/symbol.h>

SymbolInfo Environment::GetInfo(const std::string &id, bool recursive) {
    auto it = symbols_.find(id);
    if (it != symbols_.end()) {
        return it->second;
    }
    else if (outer_ && recursive) {
        return outer_->GetInfo(id);
    }
    else {
        return {SymbolType::Error, 0};
    }
}
