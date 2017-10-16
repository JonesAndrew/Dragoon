#include <math.h>

#include "value.hpp"

#define RETURN(val) vm->push(val)
#define RETURN_NUM(val) vm->push(newNum(val))
#define RETURN_STRING(val) vm->push(newString(vm, val))

void initCore(VM &vm) {
    AS(vm.memory[0], Function)->foreign = true;
    vm.functions[AS(vm.memory[0], Function)] = [](VM *vm, Value *args) {
        printf("%s\n", valueToStr(vm, args[1]).c_str());
    };

    vm.numClass->symbols[vm.compiler->findSymbol("<")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num < args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol(">")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num > args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("<=")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num <= args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol(">=")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num >= args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("==")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num == args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("!=")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num != args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("+")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num + args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("-")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num - args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("*")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num * args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("/")] = [](VM *vm, Value *args) {
        RETURN_NUM(args[0].as.num / args[1].as.num);
    };
    vm.numClass->symbols[vm.compiler->findSymbol("sin")] = [](VM *vm, Value *args) {
        RETURN_NUM(sin(args[0].as.num));
    };

    vm.strClass->symbols[vm.compiler->findSymbol("+")] = [](VM *vm, Value *args) {
        RETURN_STRING(AS(args[0], String)->value + AS(args[1], String)->value);
    };

    vm.listClass->symbols[vm.compiler->findSymbol("size")] = [](VM *vm, Value *args) {
        RETURN_NUM(AS(args[0], List)->size);
    };

    vm.listClass->symbols[vm.compiler->findSymbol("add")] = [](VM *vm, Value *args) {
        AS(args[0], List)->add(args[1]);
    };
    vm.listClass->symbols[vm.compiler->findSymbol("get")] = [](VM *vm, Value *args) {
        List *list = AS(args[0], List);
        int index = args[1].as.num;

        if (index < 0)
            index = list->size + index;

        if (index < 0 || index >= list->size)
            printf("Index out of bounds\n");

        RETURN(list->items[index]);
    };
}