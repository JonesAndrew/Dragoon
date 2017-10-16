#include "value.hpp"

Value newNum(double n) {
    Value v;
    v.isObject = false;
    v.as.num = n;

    return v;
}

String::String(VM* vm, std::string value) :
    value(value)
{
    classObject = vm->strClass;
}

Value newString(VM* vm, std::string s) {
    Value v;
    v.isObject = true;
    v.as.object = new String(vm, s);

    return v;
}

List::List(VM *vm) {
    classObject = vm->listClass;

    size = 0;
    capacity = 8;
    items = new Value[capacity];
}

void List::add(Value v) {
    if (size >= capacity) {
        capacity *= 2;
        Value *newItems = new Value[capacity];

        for (int i = 0; i < size; i++) {
            newItems[i] = items[i];
        }

        delete []items;
        items = newItems;
    }

    items[size++] = v;
}

Value newList(VM *vm) {
    Value v;
    v.isObject = true;
    v.as.object = new List(vm);

    return v;
}

Function::Function(VM *vm) :
    foreign(false)
{
    classObject = vm->functionClass;
}

Value newFunction(VM *vm) {
    Value v;
    v.isObject = true;
    v.as.object = new Function(vm);

    return v;
}
