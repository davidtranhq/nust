#include "parser/parser.h"
#include <gtest/gtest.h>
#include <sstream>

namespace nust {

TEST(ParserTest, BasicFunction) {
    std::string source = R"(
        fn main() {
            let x: i32 = 42;
            let y: bool = true;
            let z: str = "hello";
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, ArithmeticExpressions) {
    std::string source = R"(
        fn main() {
            let x: i32 = 1 + 2 * 3;
            let y: i32 = (1 + 2) * 3;
            let z: i32 = -x + y;
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, ControlFlow) {
    std::string source = R"(
        fn main() {
            let x: i32 = 42;
            if (x > 0) {
                let y: i32 = x * 2;
            } else {
                let y: i32 = 0;
            }
            
            while (x > 0) {
                x = x - 1;
            }
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, References) {
    std::string source = R"(
        fn main() {
            let x: i32 = 42;
            let y: &i32 = &x;
            let z: &mut i32 = &mut x;
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, FunctionCalls) {
    std::string source = R"(
        fn add(x: i32, y: i32) -> i32 {
            x + y
        }
        
        fn main() {
            let result: i32 = add(1, 2);
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, FunctionReturnTypes) {
    std::string source = R"(
        fn add(x: i32, y: i32) -> i32 {
            x + y
        }
        
        fn get_ref(x: &i32) -> &i32 {
            x
        }
        
        fn get_mut_ref(x: &mut i32) -> &mut i32 {
            x
        }
        
        fn main() -> i32 {
            42
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, Mutability) {
    std::string source = R"(
        fn main() {
            let mut x: i32 = 42;
            let y: i32 = 10;
            
            x = x + y;  // Should be allowed
            // y = y + x;  // Should be disallowed by type checker
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, NestedScopes) {
    std::string source = R"(
        fn main() {
            let x: i32 = 42;
            {
                let y: i32 = x;  // Can access outer scope
                let x: i32 = 10; // Shadows outer x
                {
                    let z: i32 = x + y;  // Uses inner x
                }
            }
            // y is out of scope here
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, BorrowChecking) {
    std::string source = R"(
        fn main() {
            let mut x: i32 = 42;
            let y: &i32 = &x;      // Immutable borrow
            let z: &mut i32 = &mut x;  // Mutable borrow
            
            // x = 10;  // Should be disallowed by type checker (mutable borrow active)
            // let w: &i32 = &x;  // Should be disallowed (mutable borrow active)
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, ComplexExpressions) {
    std::string source = R"(
        fn main() {
            let x: i32 = 42;
            let y: &mut i32 = &mut x;
            let z: &&i32 = &&x;
            let w: &mut &mut i32 = &mut &mut x;
            
            let a: bool = !(x == 42);
            let b: i32 = -(-x);
            let c: i32 = (1 + 2) * (3 - 4) / 5;
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

TEST(ParserTest, ErrorRecovery) {
    std::string source = R"(
        fn main() {
            let x: i32 = 42
            // Missing semicolon
            
            let y: i32 = 10;
            if (x > y) {
                let z: i32 = x + y
                // Missing semicolon
            }
            
            while (true) {
                let w: i32 = 5;
            }
        }
    )";
    
    Parser parser(source);
    auto program = parser.parse();
    ASSERT_TRUE(program != nullptr);
}

} // namespace nust 