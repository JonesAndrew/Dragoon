#include "value.hpp"

void initCore(VM &vm) {
    vm.numClass = new ObjectClass();
    vm.numClass->symbols[vm.compiler->symbolsTable["print"]] = [](VM *vm) {
        printf("%f\n", vm->pop().as.num);
    };
    vm.numClass->symbols[vm.compiler->symbolsTable["<"]] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num < a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->symbolsTable[">"]] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num > a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->symbolsTable["<="]] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num <= a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->symbolsTable[">="]] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num >= a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->symbolsTable["+"]] = [](VM *vm) {
        Value v;
        v.as.num = vm->pop().as.num + vm->pop().as.num;
        vm->push(v);
    };
    vm.numClass->symbols[vm.compiler->symbolsTable["-"]] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num - a.as.num;
        vm->push(a);
    };
    vm.numClass->symbols[vm.compiler->symbolsTable["*"]] = [](VM *vm) {
        Value v;
        v.as.num = vm->pop().as.num * vm->pop().as.num;
        vm->push(v);
    };
    vm.numClass->symbols[vm.compiler->symbolsTable["/"]] = [](VM *vm) {
        Value a = vm->pop();
        Value b = vm->pop();

        a.as.num = b.as.num / a.as.num;
        vm->push(a);
    };
}