#include "instruction.h"
#include "vm.h"

namespace nust {

bool PushInstruction::execute(VM& vm) {
    vm.push(value_);
    return true;
}

bool PopInstruction::execute(VM& vm) {
    return vm.pop().has_value();
}

bool AddInstruction::execute(VM& vm) {
    auto rhs = vm.pop();
    auto lhs = vm.pop();
    if (!rhs || !lhs) return false;

    if (lhs->is_int() && rhs->is_int()) {
        vm.push(Value(lhs->as_int() + rhs->as_int()));
        return true;
    }
    return false;
}

bool SubInstruction::execute(VM& vm) {
    auto rhs = vm.pop();
    auto lhs = vm.pop();
    if (!rhs || !lhs) return false;

    if (lhs->is_int() && rhs->is_int()) {
        vm.push(Value(lhs->as_int() - rhs->as_int()));
        return true;
    }
    return false;
}

bool MulInstruction::execute(VM& vm) {
    auto rhs = vm.pop();
    auto lhs = vm.pop();
    if (!rhs || !lhs) return false;

    if (lhs->is_int() && rhs->is_int()) {
        vm.push(Value(lhs->as_int() * rhs->as_int()));
        return true;
    }
    return false;
}

bool DivInstruction::execute(VM& vm) {
    auto rhs = vm.pop();
    auto lhs = vm.pop();
    if (!rhs || !lhs) return false;

    if (lhs->is_int() && rhs->is_int()) {
        if (rhs->as_int() == 0) return false;
        vm.push(Value(lhs->as_int() / rhs->as_int()));
        return true;
    }
    return false;
}

bool EqInstruction::execute(VM& vm) {
    auto rhs = vm.pop();
    auto lhs = vm.pop();
    if (!rhs || !lhs) return false;

    if (lhs->is_int() && rhs->is_int()) {
        vm.push(Value(lhs->as_int() == rhs->as_int()));
        return true;
    }
    if (lhs->is_bool() && rhs->is_bool()) {
        vm.push(Value(lhs->as_bool() == rhs->as_bool()));
        return true;
    }
    if (lhs->is_string() && rhs->is_string()) {
        vm.push(Value(lhs->as_string() == rhs->as_string()));
        return true;
    }
    return false;
}

bool NeqInstruction::execute(VM& vm) {
    auto rhs = vm.pop();
    auto lhs = vm.pop();
    if (!rhs || !lhs) return false;

    if (lhs->is_int() && rhs->is_int()) {
        vm.push(Value(lhs->as_int() != rhs->as_int()));
        return true;
    }
    if (lhs->is_bool() && rhs->is_bool()) {
        vm.push(Value(lhs->as_bool() != rhs->as_bool()));
        return true;
    }
    if (lhs->is_string() && rhs->is_string()) {
        vm.push(Value(lhs->as_string() != rhs->as_string()));
        return true;
    }
    return false;
}

bool LoadGlobalInstruction::execute(VM& vm) {
    if (index_ >= vm.globals_size()) return false;
    vm.push(vm.get_global(index_));
    return true;
}

bool StoreGlobalInstruction::execute(VM& vm) {
    auto value = vm.pop();
    if (!value) return false;
    if (index_ >= vm.globals_size()) {
        vm.resize_globals(index_ + 1);
    }
    vm.set_global(index_, *value);
    return true;
}

} // namespace nust 