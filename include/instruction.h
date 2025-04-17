#ifndef NUST_INSTRUCTION_H
#define NUST_INSTRUCTION_H

#include "value.h"
#include <vector>
#include <memory>

namespace nust {

class VM; // Forward declaration

class Instruction {
public:
    virtual ~Instruction() = default;
    virtual bool execute(VM& vm) = 0;
};

// Push a constant value onto the stack
class PushInstruction : public Instruction {
public:
    PushInstruction(Value value) : value_(value) {}
    bool execute(VM& vm) override;

private:
    Value value_;
};

// Pop a value from the stack
class PopInstruction : public Instruction {
public:
    bool execute(VM& vm) override;
};

// Add two values from the stack
class AddInstruction : public Instruction {
public:
    bool execute(VM& vm) override;
};

// Subtract two values from the stack
class SubInstruction : public Instruction {
public:
    bool execute(VM& vm) override;
};

// Multiply two values from the stack
class MulInstruction : public Instruction {
public:
    bool execute(VM& vm) override;
};

// Divide two values from the stack
class DivInstruction : public Instruction {
public:
    bool execute(VM& vm) override;
};

// Compare two values for equality
class EqInstruction : public Instruction {
public:
    bool execute(VM& vm) override;
};

// Compare two values for inequality
class NeqInstruction : public Instruction {
public:
    bool execute(VM& vm) override;
};

// Load a value from a global variable
class LoadGlobalInstruction : public Instruction {
public:
    LoadGlobalInstruction(size_t index) : index_(index) {}
    bool execute(VM& vm) override;

private:
    size_t index_;
};

// Store a value to a global variable
class StoreGlobalInstruction : public Instruction {
public:
    StoreGlobalInstruction(size_t index) : index_(index) {}
    bool execute(VM& vm) override;

private:
    size_t index_;
};

} // namespace nust

#endif // NUST_INSTRUCTION_H 