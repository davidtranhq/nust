#include <iostream>
#include <fstream>
#include <sstream>
#include "parser/parser.h"
#include "type_checker.h"
#include "compiler.h"
#include "vm.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }
    
    // Read source file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << "\n";
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    try {
        // Parse source code
        nust::Parser parser(source);
        auto program = parser.parse();
        
        // Type check
        nust::TypeChecker type_checker(*program);
        if (!type_checker.check()) {
            std::cerr << "Type checking failed\n";
            return 1;
        }
        
        // Compile to bytecode
        nust::Compiler compiler;
        auto instructions = compiler.compile(*program);
        
        // Convert instructions to bytecode
        std::vector<uint8_t> bytecode;
        for (const auto& instr : instructions) {
            bytecode.push_back(static_cast<uint8_t>(instr.opcode));
            if (instr.has_operand()) {
                // Encode operand as little-endian
                for (size_t i = 0; i < sizeof(size_t); ++i) {
                    bytecode.push_back((instr.operand >> (i * 8)) & 0xFF);
                }
            }
        }
        
        // Execute bytecode
        nust::VM vm;
        if (!vm.load_bytecode(bytecode)) {
            std::cerr << "Failed to load bytecode\n";
            return 1;
        }
        
        std::cout << "Executing program...\n";
        if (!vm.execute()) {
            std::cerr << "Failed to execute bytecode\n";
            return 1;
        }
        
        std::cout << "Execution completed successfully\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
} 