#ifndef NUST_VM_H
#define NUST_VM_H

#include "value.h"
#include "instruction.h"
#include "function_table.h"
#include <vector>
#include <stack>
#include <memory>
#include <stdexcept>

namespace nust {

class VirtualMachine {
public:
    // Constructor
    VirtualMachine(const FunctionTable& function_table, 
                  const std::vector<Value>& constants,
                  const std::vector<Instruction>& instructions);

    // Run the VM
    void run();

    // Get the result of execution
    Value get_result() const;

private:
    // VM state
    const FunctionTable& function_table_;
    const std::vector<Value>& constants_;
    const std::vector<Instruction>& instructions_;
    
    // Runtime state
    std::vector<Value> memory_;  // Contiguous memory for all runtime data
    std::vector<Value> stack_;    // Operand stack
    size_t pc_;                  // Program counter
    size_t fp_;                  // Frame pointer
    Value result_;               // Result of execution
    bool running_;               // Whether the VM is running
    bool returned_from_main_;     // Whether the main function has returned

    // Helper methods
    void execute_instruction(const Instruction& instr);
    void push(const Value& value);
    Value pop();
    Value& top();
    void check_stack_size(size_t required) const;
    void check_memory_bounds(size_t index) const;
    
    // Instruction handlers
    void handle_push_i32(size_t operand);
    void handle_push_bool(size_t operand);
    void handle_push_str(size_t operand);
    void handle_pop();
    void handle_dup();
    void handle_swap();
    void handle_load(size_t operand);
    void handle_store(size_t operand);
    void handle_load_ref(size_t operand);
    void handle_store_ref();
    void handle_add_i32();
    void handle_sub_i32();
    void handle_mul_i32();
    void handle_div_i32();
    void handle_neg_i32();
    void handle_eq_i32();
    void handle_ne_i32();
    void handle_lt_i32();
    void handle_gt_i32();
    void handle_le_i32();
    void handle_ge_i32();
    void handle_and();
    void handle_or();
    void handle_not();
    void handle_jmp(size_t operand);
    void handle_jmp_if(size_t operand);
    void handle_jmp_if_not(size_t operand);
    void handle_call(size_t operand);
    void handle_ret();
    void handle_ret_val();
    void handle_borrow();
    void handle_borrow_mut();
    void handle_deref();
    void handle_deref_mut();
};

} // namespace nust

#endif // NUST_VM_H 