#include <gtest/gtest.h>
#include "compiler.h"
#include "parser/parser.h"
#include "vm.h"
#include <sstream>
#include <string>

using namespace nust;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test data
        constants_ = {
            Value(42),      // 0: integer constant
            Value(true),    // 1: boolean constant
            Value("test")   // 2: string constant
        };
    }

    Value run_program(const std::string& source) {
        // Parse the source code
        Parser parser(source);
        auto program = parser.parse();
        
        // Compile the program
        Compiler compiler;
        auto instructions = compiler.compile(*program);
        
        // Run the program
        VirtualMachine vm(compiler.get_function_table(), constants_, instructions);
        vm.run();
        return vm.get_result();
    }

    std::vector<Value> constants_;
};

// Test basic arithmetic
TEST_F(IntegrationTest, BasicArithmetic) {
    const char* source = R"(
        fn main() -> i32 {
            let x: i32 = 42;
            let y: i32 = 2;
            return x + y;
        }
    )";
    
    Value result = run_program(source);
    EXPECT_EQ(result.as_int(), 44);
}

// Test control flow
TEST_F(IntegrationTest, ControlFlow) {
    const char* source = R"(
        fn main() -> i32 {
            let x: i32 = 42;
            if (x > 0) {
                return x + 1;
            } else {
                return x - 1;
            }
        }
    )";
    
    Value result = run_program(source);
    EXPECT_EQ(result.as_int(), 43);
}

// Test function calls
TEST_F(IntegrationTest, FunctionCalls) {
    const char* source = R"(
        fn add(x: i32, y: i32) -> i32 {
            return x + y;
        }
        
        fn main() -> i32 {
            let result: i32 = add(40, 2);
            return result;
        }
    )";
    
    Value result = run_program(source);
    EXPECT_EQ(result.as_int(), 42);
}


// Test while loop
TEST_F(IntegrationTest, WhileLoop) {
    const char* source = R"(
        fn main() -> i32 {
            let mut x: i32 = 0;
            while (x < 10) {
                x = x + 1;
            }
            return x;
        }
    )";
    
    Value result = run_program(source);
    EXPECT_EQ(result.as_int(), 10);
}
