#include "value.hpp"

Value newNum(double n) {
    Value v;
    v.isObject = false;
    v.as.num = n;

    return v;
}

String *initString(VM* vm, std::string s) {
    String *str = new String();
    str->classObject = vm->strClass;
    str->value = s;

    return str;
}

Value newString(VM* vm, std::string s) {
    Value v;
    v.isObject = true;
    v.as.object = initString(vm, s);

    return v;
}

List *initList(VM *vm) {
    List *list = new List();
    list->classObject = vm->listClass;

    list->size = 0;
    list->capacity = 8;
    list->items = new Value[list->capacity];

    return list;
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
    v.as.object = initList(vm);

    return v;
}
