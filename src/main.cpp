#include <iostream>
#include "vm.h"

int main(int argc, char* argv[]) {
    nust::VM vm;
    
    // Create some test bytecode
    std::vector<uint8_t> test_bytecode = {
        // TODO: Add actual bytecode instructions
    };

    std::cout << "Loading bytecode...\n";
    if (!vm.load_bytecode(test_bytecode)) {
        std::cerr << "Failed to load bytecode\n";
        return 1;
    }

    std::cout << "Executing bytecode...\n";
    if (!vm.execute()) {
        std::cerr << "Failed to execute bytecode\n";
        return 1;
    }

    std::cout << "Execution completed successfully\n";
    return 0;
} 