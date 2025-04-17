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

} // namespace nust 