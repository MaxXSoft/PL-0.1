#include <define/symbol.h>

SymbolType Environment::GetType(const std::string &id) {
    auto it = symbols_.find(id);
    if (it != symbols_.end()) {
        return it->second.type;
    }
    else if (outer_) {
        return outer_->GetType(id);
    }
    else {
        return SymbolType::Error;
    }
}
