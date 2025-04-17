#include "vm.h"
#include <iostream>

namespace nust {

VM::VM() {
    // Initialize VM state
}

VM::~VM() {
    // Clean up resources
}

bool VM::load_bytecode(const std::vector<uint8_t>& bytecode) {
    // TODO: Implement bytecode loading
    return false;
}

bool VM::execute() {
    for (const auto& instruction : instructions_) {
        if (!instruction->execute(*this)) {
            std::cerr << "Error executing instruction\n";
            return false;
        }
    }
    return true;
}

void VM::push(const Value& value) {
    stack_.push_back(value);
}

std::optional<Value> VM::pop() {
    if (stack_.empty()) {
        return std::nullopt;
    }
    Value value = stack_.back();
    stack_.pop_back();
    return value;
}

} // namespace nust 