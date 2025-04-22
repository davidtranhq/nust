#include "vm.h"
#include <stdexcept>
#include <cassert>
#include <iostream>

namespace nust {

VirtualMachine::VirtualMachine(const FunctionTable& function_table,
                             const std::vector<Value>& constants,
                             const std::vector<Instruction>& instructions)
    : function_table_(function_table)
    , constants_(constants)
    , instructions_(instructions)
    , memory_(1024)  // Start with 1KB of memory
    , pc_(0)
    , fp_(0)
    , running_(true)
    , returned_from_main_(false)
{
    // Initialize memory with a reasonable size
    memory_.resize(1024);  // Can be adjusted based on needs
    
    // Find main function and set up initial call
    size_t main_index = function_table_.get_function_index("main");
    if (main_index == function_table_.size()) {
        throw std::runtime_error("No main() function found");
    }
    
    const auto& main_func = function_table_.get_function(main_index);
    if (main_func.num_params != 0) {
        throw std::runtime_error("main() function must take no parameters");
    }
    
    
    // Jump to main's entry point
    pc_ = main_func.entry_point;  // -1 because pc_ will be incremented in run()
}

void VirtualMachine::run() {
    while (running_ && pc_ < instructions_.size()) {
        execute_instruction(instructions_[pc_]);
        pc_++;
    }

    if (!returned_from_main_) {
        result_ = stack_.back();
    }
}

Value VirtualMachine::get_result() const {
    return result_;
}

void VirtualMachine::execute_instruction(const Instruction& instr) {
    switch (instr.opcode) {
        case Opcode::PUSH_I32:
            handle_push_i32(instr.operand);
            break;
        case Opcode::PUSH_BOOL:
            handle_push_bool(instr.operand);
            break;
        case Opcode::PUSH_STR:
            handle_push_str(instr.operand);
            break;
        case Opcode::POP:
            handle_pop();
            break;
        case Opcode::DUP:
            handle_dup();
            break;
        case Opcode::SWAP:
            handle_swap();
            break;
        case Opcode::LOAD:
            handle_load(instr.operand);
            break;
        case Opcode::STORE:
            handle_store(instr.operand);
            break;
        case Opcode::LOAD_REF:
            handle_load_ref(instr.operand);
            break;
        case Opcode::STORE_REF:
            handle_store_ref();
            break;
        case Opcode::ADD_I32:
            handle_add_i32();
            break;
        case Opcode::SUB_I32:
            handle_sub_i32();
            break;
        case Opcode::MUL_I32:
            handle_mul_i32();
            break;
        case Opcode::DIV_I32:
            handle_div_i32();
            break;
        case Opcode::NEG_I32:
            handle_neg_i32();
            break;
        case Opcode::EQ_I32:
            handle_eq_i32();
            break;
        case Opcode::NE_I32:
            handle_ne_i32();
            break;
        case Opcode::LT_I32:
            handle_lt_i32();
            break;
        case Opcode::GT_I32:
            handle_gt_i32();
            break;
        case Opcode::LE_I32:
            handle_le_i32();
            break;
        case Opcode::GE_I32:
            handle_ge_i32();
            break;
        case Opcode::AND:
            handle_and();
            break;
        case Opcode::OR:
            handle_or();
            break;
        case Opcode::NOT:
            handle_not();
            break;
        case Opcode::JMP:
            handle_jmp(instr.operand);
            break;
        case Opcode::JMP_IF:
            handle_jmp_if(instr.operand);
            break;
        case Opcode::JMP_IF_NOT:
            handle_jmp_if_not(instr.operand);
            break;
        case Opcode::CALL:
            handle_call(instr.operand);
            break;
        case Opcode::RET:
            handle_ret();
            break;
        case Opcode::RET_VAL:
            handle_ret_val();
            break;
        case Opcode::BORROW:
            handle_borrow();
            break;
        case Opcode::BORROW_MUT:
            handle_borrow_mut();
            break;
        case Opcode::DEREF:
            handle_deref();
            break;
        case Opcode::DEREF_MUT:
            handle_deref_mut();
            break;
        default:
            throw std::runtime_error("Unknown opcode");
    }
}

void VirtualMachine::push(const Value& value) {
    stack_.push_back(value);
}

Value VirtualMachine::pop() {
    if (stack_.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    Value value = stack_.back();;
    stack_.pop_back();
    return value;
}

Value& VirtualMachine::top() {
    if (stack_.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    return stack_.back();;
}

void VirtualMachine::check_stack_size(size_t required) const {
    if (stack_.size() < required) {
        throw std::runtime_error("Stack underflow");
    }
}

void VirtualMachine::check_memory_bounds(size_t index) const {
    if (index >= memory_.size()) {
        throw std::runtime_error("Memory access out of bounds");
    }
}

// Stack operations
void VirtualMachine::handle_push_i32(size_t operand) {
    push(Value(static_cast<Value::IntType>(operand)));
}

void VirtualMachine::handle_push_bool(size_t operand) {
    push(Value(static_cast<Value::BoolType>(operand != 0)));
}

void VirtualMachine::handle_push_str(size_t operand) {
    if (operand >= constants_.size()) {
        throw std::runtime_error("String constant index out of bounds");
    }
    push(constants_[operand]);
}

void VirtualMachine::handle_pop() {
    pop();
}

void VirtualMachine::handle_dup() {
    check_stack_size(1);
    push(top());
}

void VirtualMachine::handle_swap() {
    check_stack_size(2);
    Value a = pop();
    Value b = pop();
    push(a);
    push(b);
}

// Variable operations
void VirtualMachine::handle_load(size_t operand) {
    check_memory_bounds(fp_ + operand);
    push(memory_[fp_ + operand]);
}

void VirtualMachine::handle_store(size_t operand) {
    check_stack_size(1);
    check_memory_bounds(fp_ + operand);
    memory_[fp_ + operand] = pop();
}

void VirtualMachine::handle_load_ref(size_t operand) {
    check_memory_bounds(fp_ + operand);
    push(Value(std::make_shared<Value>(memory_[fp_ + operand])));
}

void VirtualMachine::handle_store_ref() {
    check_stack_size(2);
    Value ref = pop();
    Value value = pop();
    if (!ref.is_ref()) {
        throw std::runtime_error("Expected reference value");
    }
    *ref.as_ref() = value;
}

// Arithmetic operations
void VirtualMachine::handle_add_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() + b.as_int()));
}

void VirtualMachine::handle_sub_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() - b.as_int()));
}

void VirtualMachine::handle_mul_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() * b.as_int()));
}

void VirtualMachine::handle_div_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    if (b.as_int() == 0) {
        throw std::runtime_error("Division by zero");
    }
    push(Value(a.as_int() / b.as_int()));
}

void VirtualMachine::handle_neg_i32() {
    check_stack_size(1);
    Value a = pop();
    if (!a.is_int()) {
        throw std::runtime_error("Expected integer value");
    }
    push(Value(-a.as_int()));
}

// Comparison operations
void VirtualMachine::handle_eq_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() == b.as_int()));
}

void VirtualMachine::handle_ne_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() != b.as_int()));
}

void VirtualMachine::handle_lt_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() < b.as_int()));
}

void VirtualMachine::handle_gt_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() > b.as_int()));
}

void VirtualMachine::handle_le_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() <= b.as_int()));
}

void VirtualMachine::handle_ge_i32() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_int() || !b.is_int()) {
        throw std::runtime_error("Expected integer values");
    }
    push(Value(a.as_int() >= b.as_int()));
}

// Logical operations
void VirtualMachine::handle_and() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_bool() || !b.is_bool()) {
        throw std::runtime_error("Expected boolean values");
    }
    push(Value(a.as_bool() && b.as_bool()));
}

void VirtualMachine::handle_or() {
    check_stack_size(2);
    Value b = pop();
    Value a = pop();
    if (!a.is_bool() || !b.is_bool()) {
        throw std::runtime_error("Expected boolean values");
    }
    push(Value(a.as_bool() || b.as_bool()));
}

void VirtualMachine::handle_not() {
    check_stack_size(1);
    Value a = pop();
    if (!a.is_bool()) {
        throw std::runtime_error("Expected boolean value");
    }
    push(Value(!a.as_bool()));
}

// Control flow
void VirtualMachine::handle_jmp(size_t operand) {
    pc_ = operand - 1;  // -1 because pc_ will be incremented after this instruction
}

void VirtualMachine::handle_jmp_if(size_t operand) {
    check_stack_size(1);
    Value cond = pop();
    if (!cond.is_bool()) {
        throw std::runtime_error("Expected boolean value");
    }
    if (cond.as_bool()) {
        pc_ = operand - 1;
    }
}

void VirtualMachine::handle_jmp_if_not(size_t operand) {
    check_stack_size(1);
    Value cond = pop();
    if (!cond.is_bool()) {
        throw std::runtime_error("Expected boolean value");
    }
    if (!cond.as_bool()) {
        pc_ = operand - 1;
    }
}

void VirtualMachine::handle_call(size_t operand) {
    const auto& func_info = function_table_.get_function(operand);
    if (operand >= function_table_.size()) {
        throw std::runtime_error("Function index out of bounds");
    }
    
    if (stack_.size() < func_info.num_params) {
        throw std::runtime_error("Not enough arguments for function call");
    }
    
    // Save return address and frame pointer
    memory_[fp_] = Value(static_cast<Value::IntType>(pc_ + 1));
    memory_[fp_ + 1] = Value(static_cast<Value::IntType>(fp_));
    
    // Set up new frame
    size_t old_fp = fp_;
    fp_ += 2 + func_info.num_locals;  // Skip return address and frame pointer
    
    // Copy arguments to new frame in correct order
    std::vector<Value> args;
    for (size_t i = 0; i < func_info.num_params; ++i) {
        args.push_back(pop());
    }
    for (size_t i = 0; i < func_info.num_params; ++i) {
        memory_[fp_ + i] = args[func_info.num_params - 1 - i];
    }
    
    // Jump to function
    pc_ = func_info.entry_point - 1;
}

void VirtualMachine::handle_ret() {
    // If we're returning from main (fp_ is 0), stop the VM
    if (fp_ == 0) {
        running_ = false;
        returned_from_main_ = true;
        return;
    }

    // Restore frame pointer
    fp_ = static_cast<size_t>(memory_[fp_ + 1].as_int());

    // Restore program counter
    pc_ = static_cast<size_t>(memory_[fp_].as_int()) - 1;
}

void VirtualMachine::handle_ret_val() {
    check_stack_size(1);
    Value ret_val = pop();

    // If we're returning from main (fp_ is 0), stop the VM
    if (fp_ == 0) {
        result_ = ret_val;
        running_ = false;
        returned_from_main_ = true;
        return;
    }
    // Restore frame pointer
    fp_ = static_cast<size_t>(memory_[fp_ + 1].as_int());
    // Restore program counter
    pc_ = static_cast<size_t>(memory_[fp_].as_int()) - 1;
    // Push return value for caller
    push(ret_val);
}

// Reference operations
void VirtualMachine::handle_borrow() {
    check_stack_size(1);
    Value value = pop();
    push(Value(std::make_shared<Value>(value)));
}

void VirtualMachine::handle_borrow_mut() {
    check_stack_size(1);
    Value value = pop();
    push(Value(std::make_shared<Value>(value)));
}

void VirtualMachine::handle_deref() {
    check_stack_size(1);
    Value ref = pop();
    if (!ref.is_ref()) {
        throw std::runtime_error("Expected reference value");
    }
    push(*ref.as_ref());
}

void VirtualMachine::handle_deref_mut() {
    check_stack_size(1);
    Value ref = pop();
    if (!ref.is_ref()) {
        throw std::runtime_error("Expected reference value");
    }
    push(*ref.as_ref());
}

} // namespace nust 