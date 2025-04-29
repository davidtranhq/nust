#include <gtest/gtest.h>
#include "vm.h"
#include "value.h"
#include "instruction.h"
#include "function_table.h"
#include "parser.h"
#include <vector>
#include <memory>

using namespace nust;

class VMTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test data
        constants_ = {
            Value(42),      // 0: integer constant
            Value(true),    // 1: boolean constant
            Value("test")   // 2: string constant
        };

        // Create a simple function table with a main function
        auto main_decl = std::make_unique<FunctionDecl>(
            Span(0, 0),
            "main",
            std::vector<FunctionDecl::Param>{},
            std::make_unique<Type>(Type::Kind::I32, Span(0, 0)),
            nullptr
        );
        function_table_.add_function(*main_decl, 0);
    }

    std::vector<Value> constants_;
    FunctionTable function_table_;
};

// Test basic stack operations
TEST_F(VMTest, StackOperations) {
    std::vector<Instruction> instructions = {
        {Opcode::PUSH_I32, 42},  // Push 42
        {Opcode::DUP},          // Duplicate top
        {Opcode::POP},          // Pop one value
        {Opcode::PUSH_I32, 42},  // Push 42 again
        {Opcode::PUSH_I32, 42},  // Push 42 again
        {Opcode::SWAP}          // Swap top two values
    };

    VirtualMachine vm(function_table_, constants_, instructions);
    vm.run();
    EXPECT_EQ(vm.get_result().as_int(), 42);
}

// Test arithmetic operations
TEST_F(VMTest, ArithmeticOperations) {
    std::vector<Instruction> instructions = {
        {Opcode::PUSH_I32, 42},  // Push 42
        {Opcode::PUSH_I32, 42},  // Push 42
        {Opcode::ADD_I32},      // 42 + 42 = 84
        {Opcode::PUSH_I32, 42},  // Push 42
        {Opcode::SUB_I32},      // 84 - 42 = 42
        {Opcode::PUSH_I32, 42},  // Push 42
        {Opcode::MUL_I32},      // 42 * 42 = 1764
        {Opcode::PUSH_I32, 42},  // Push 42
        {Opcode::DIV_I32},      // 1764 / 42 = 42
        {Opcode::NEG_I32}       // -42
    };

    VirtualMachine vm(function_table_, constants_, instructions);
    vm.run();
    EXPECT_EQ(vm.get_result().as_int(), -42);
}

// Test comparison operations
TEST_F(VMTest, ComparisonOperations) {
    std::vector<Instruction> instructions = {
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::EQ_I32},       // 42 == 42 = true
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::NE_I32},       // 42 != 42 = false
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::LT_I32},       // 42 < 42 = false
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::GT_I32},       // 42 > 42 = false
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::LE_I32},       // 42 <= 42 = true
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::GE_I32}        // 42 >= 42 = true
    };

    VirtualMachine vm(function_table_, constants_, instructions);
    vm.run();
    EXPECT_TRUE(vm.get_result().as_bool());
}

// Test logical operations
TEST_F(VMTest, LogicalOperations) {
    std::vector<Instruction> instructions = {
        {Opcode::PUSH_BOOL, 1},  // Push true
        {Opcode::PUSH_BOOL, 1},  // Push true
        {Opcode::AND},           // true && true = true
        {Opcode::PUSH_BOOL, 1},  // Push true
        {Opcode::PUSH_BOOL, 0},  // Push false
        {Opcode::OR},            // true || false = true
        {Opcode::NOT}            // !true = false
    };

    VirtualMachine vm(function_table_, constants_, instructions);
    vm.run();
    EXPECT_FALSE(vm.get_result().as_bool());
}

// Test control flow
TEST_F(VMTest, ControlFlow) {
    std::vector<Instruction> instructions = {
        {Opcode::PUSH_I32, 1}, 
        {Opcode::PUSH_BOOL, 1}, // Push true
        {Opcode::JMP_IF, 6},    // Jump to instruction 6 if true
        {Opcode::PUSH_I32, 2},  // This should be skipped
        {Opcode::PUSH_I32, 3},  // This should be skipped

    };

    VirtualMachine vm(function_table_, constants_, instructions);
    vm.run();
    EXPECT_EQ(vm.get_result().as_int(), 1);
}

// Test function calls
TEST_F(VMTest, FunctionCalls) {
    // Add a test function to the function table
    std::vector<FunctionDecl::Param> params;
    params.emplace_back(false, "x", std::make_unique<Type>(Type::Kind::I32, Span(0, 0)), Span(0, 0));
    
    auto test_decl = std::make_unique<FunctionDecl>(
        Span(0, 0),
        "test",
        std::move(params),
        std::make_unique<Type>(Type::Kind::I32, Span(0, 0)),
        nullptr
    );
    size_t test_func_idx = function_table_.add_function(*test_decl, 3);

    // Main function:
    // 1. Push argument (42)
    // 2. Call test function
    // 3. Return value from test function
    // Test function:
    // 1. Load argument (x)
    // 2. Return value
    std::vector<Instruction> instructions = {
        // Main function
        {Opcode::PUSH_I32, 42},      // Push 42
        {Opcode::CALL, test_func_idx}, // Call test function
        {Opcode::RET_VAL},           // Return value from main function
        
        // Test function
        {Opcode::LOAD, 0},           // Load argument x (offset 0 in new frame)
        {Opcode::RET_VAL}            // Return value from test function
    };

    VirtualMachine vm(function_table_, constants_, instructions);
    vm.run();
    EXPECT_EQ(vm.get_result().as_int(), 42);
}

// Test reference operations
TEST_F(VMTest, ReferenceOperations) {
    std::vector<Instruction> instructions = {
        {Opcode::PUSH_I32, 42},   // Push 42
        {Opcode::BORROW},        // Create reference
        {Opcode::DEREF},         // Dereference
        {Opcode::PUSH_I32, 42},   // Push 42
        {Opcode::BORROW_MUT},    // Create mutable reference
        {Opcode::DEREF_MUT}      // Dereference mutable
    };

    VirtualMachine vm(function_table_, constants_, instructions);
    vm.run();
    EXPECT_EQ(vm.get_result().as_int(), 42);
}

// Test error handling
TEST_F(VMTest, ErrorHandling) {
    // Test stack underflow
    std::vector<Instruction> underflow_instructions = {
        {Opcode::POP}  // Try to pop from empty stack
    };

    VirtualMachine vm1(function_table_, constants_, underflow_instructions);
    EXPECT_THROW(vm1.run(), std::runtime_error);

    // Test division by zero
    std::vector<Instruction> div_zero_instructions = {
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::SUB_I32},      // 42 - 42 = 0
        {Opcode::PUSH_I32, 0},  // Push 42
        {Opcode::DIV_I32}       // 42 / 0
    };

    VirtualMachine vm2(function_table_, constants_, div_zero_instructions);
    EXPECT_THROW(vm2.run(), std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 