#include <define/symbol.h>

SymbolInfo Environment::GetInfo(const std::string &id) {
    auto it = symbols_.find(id);
    if (it != symbols_.end()) {
        return it->second;
    }
    else if (outer_) {
        return outer_->GetInfo(id);
    }
    else {
        return {SymbolType::Error, 0};
    }
}
