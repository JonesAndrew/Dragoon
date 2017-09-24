#include "value.hpp"

void initCore(VM &vm) {
    vm.numClass->symbols[vm.compiler->findSymbol("<")] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num < a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->findSymbol(">")] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num > a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("<=")] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num <= a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->findSymbol(">=")] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num >= a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("+")] = [](VM *vm) {
        Value v;
        v.as.num = vm->pop().as.num + vm->pop().as.num;
        vm->push(v);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("-")] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num - a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("*")] = [](VM *vm) {
        Value v;
        v.as.num = vm->pop().as.num * vm->pop().as.num;
        vm->push(v);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("/")] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num / a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("print")] = [](VM *vm) {
        Value a = vm->pop();
        
        printf("%s\n", valueToStr(vm, a).c_str());
    };

    vm.strClass->symbols[vm.compiler->findSymbol("+")] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        vm->push(newString(vm, AS(b, String)->value + AS(a, String)->value));
    };

    vm.listClass->symbols[vm.compiler->findSymbol("size")] = [](VM *vm) {
        vm->push(newNum(AS(vm->pop(), List)->size));
    };

    vm.listClass->symbols[vm.compiler->findSymbol("add")] = [](VM *vm) {
        Value v = vm->pop();
        AS(vm->pop(), List)->add(v);
    };
}