#ifndef NUST_VM_H
#define NUST_VM_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "value.h"
#include "instruction.h"

namespace nust {

// Forward declarations
class Value;
class Instruction;

class VM {
public:
    VM();
    ~VM();

    // Load and execute bytecode
    bool load_bytecode(const std::vector<uint8_t>& bytecode);
    bool execute();

    // Stack operations
    void push(const Value& value);
    std::optional<Value> pop();
    size_t stack_size() const { return stack_.size(); }

    // Global variable operations
    const Value& get_global(size_t index) const { return globals_[index]; }
    void set_global(size_t index, const Value& value) { globals_[index] = value; }
    size_t globals_size() const { return globals_.size(); }
    void resize_globals(size_t new_size) { globals_.resize(new_size); }

private:
    std::vector<Value> stack_;
    std::vector<Value> globals_;
    std::vector<std::unique_ptr<Instruction>> instructions_;
};

} // namespace nust

#endif // NUST_VM_H 