#include <gtest/gtest.h>
#include "vm.h"
#include "instruction.h"

using namespace nust;

class VmTest : public ::testing::Test {
protected:
    VM vm;
};

TEST_F(VmTest, StackOperations) {
    // Test push and pop
    vm.push(Value(42));
    EXPECT_EQ(vm.stack_size(), 1);
    
    auto value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_int());
    EXPECT_EQ(value->as_int(), 42);
    EXPECT_EQ(vm.stack_size(), 0);
    
    // Test empty pop
    value = vm.pop();
    EXPECT_FALSE(value.has_value());
}

TEST_F(VmTest, GlobalOperations) {
    // Test global variable operations
    vm.push(Value(100));
    vm.push(Value(200));
    
    // Store values in globals
    StoreGlobalInstruction store0(0);
    StoreGlobalInstruction store1(1);
    
    EXPECT_TRUE(store0.execute(vm));
    EXPECT_TRUE(store1.execute(vm));
    
    // Load values from globals
    LoadGlobalInstruction load0(0);
    LoadGlobalInstruction load1(1);
    
    EXPECT_TRUE(load0.execute(vm));
    auto value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_int());
    EXPECT_EQ(value->as_int(), 200);
    
    EXPECT_TRUE(load1.execute(vm));
    value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_int());
    EXPECT_EQ(value->as_int(), 100);
}

TEST_F(VmTest, ArithmeticOperations) {
    // Test addition
    vm.push(Value(10));
    vm.push(Value(20));
    AddInstruction add;
    EXPECT_TRUE(add.execute(vm));
    auto value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_int());
    EXPECT_EQ(value->as_int(), 30);
    
    // Test subtraction
    vm.push(Value(30));
    vm.push(Value(10));
    SubInstruction sub;
    EXPECT_TRUE(sub.execute(vm));
    value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_int());
    EXPECT_EQ(value->as_int(), 20);
    
    // Test multiplication
    vm.push(Value(5));
    vm.push(Value(6));
    MulInstruction mul;
    EXPECT_TRUE(mul.execute(vm));
    value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_int());
    EXPECT_EQ(value->as_int(), 30);
    
    // Test division
    vm.push(Value(20));
    vm.push(Value(5));
    DivInstruction div;
    EXPECT_TRUE(div.execute(vm));
    value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_int());
    EXPECT_EQ(value->as_int(), 4);
}

TEST_F(VmTest, ComparisonOperations) {
    // Test equality
    vm.push(Value(10));
    vm.push(Value(10));
    EqInstruction eq;
    EXPECT_TRUE(eq.execute(vm));
    auto value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_bool());
    EXPECT_TRUE(value->as_bool());
    
    // Test inequality
    vm.push(Value(10));
    vm.push(Value(20));
    NeqInstruction neq;
    EXPECT_TRUE(neq.execute(vm));
    value = vm.pop();
    ASSERT_TRUE(value.has_value());
    EXPECT_TRUE(value->is_bool());
    EXPECT_TRUE(value->as_bool());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 