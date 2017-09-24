#pragma once

#include <map>
#include <string>
#include <vector>
#include <stack>

#define AS(value, type) static_cast<type *>(value.as.object)

struct Value;

struct ObjectClass;

class VM;

std::string valueToStr(VM *vm, Value v);

enum ObjectType
{
    STRING,
};

struct Object {
    ObjectType objectType;

    ObjectClass *classObject;

    std::vector<Value> fields;
};

struct String : public Object {
    std::string value;
};

struct List : public Object {
    Value *items;

    int capacity;
    int size;

    void add(Value v);
};

struct Value {
    union {
        double num;
        Object *object;
    } as;
    bool isObject;
};

Value newNum(double n);

String *initString(VM* vm, std::string s);

Value newString(VM* vm, std::string s);

List *initList(VM *vm);

Value newList(VM *vm);


enum TokenType {
    TOKEN_EMPTY,

    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENT,
    TOKEN_SYMBOL,
    TOKEN_SYMBOL_START,

    TOKEN_TRUE,
    TOKEN_FALSE,

    TOKEN_DELETE,

    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_MUL,
    TOKEN_DIV,

    TOKEN_EQ,

    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTEQ,
    TOKEN_GTEQ,
    TOKEN_EQEQ,
    TOKEN_NEQ,

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURLY,
    TOKEN_RCURLY,

    TOKEN_LBRACKET,
    TOKEN_RBRACKET,

    TOKEN_IF,
    TOKEN_ELSE,

    TOKEN_WHILE,

    TOKEN_COMMA,
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType type, std::string value = "") :
        type(type),
        value(value) {}
};

class Compiler {
    VM *vm;

    std::vector<Token> input;
    int it;

    Token current;
    Token next;

    std::map<std::string, int> vars;
    std::vector<uint8_t> code;

    int varOffset;

    void consume();
    void match(TokenType type);

    void term();

    bool isAddop(TokenType type);
    bool isRelop(TokenType type);

    void ifBlock();
    void whileBlock();
    void deleteStatment();
    void statement();
    void function();
    void block();
    void expression();
    void assignment();

    void add();
    void sub();

    void factor();

    void mul();
    void div();

public:
    std::vector<Value> constants;
    std::map<std::string, int> symbolsTable;

    Compiler(VM *vm);

    uint8_t findSymbol(std::string symbol);
    std::vector<uint8_t> compile(std::vector<Token> in);
};

struct ObjectClass {
    std::map<int, std::function<void(VM *vm, Value *args)> > symbols;
};

class VM {
    std::vector<Value> memory;

public:
    Compiler *compiler;
    ObjectClass *numClass;
    ObjectClass *strClass;
    ObjectClass *listClass;

    std::vector<Value> stack;

    VM();

    Value pop();

    void push(Value val);

    void printStack();

    void run(std::vector<Token> input);

    void callMethod(uint8_t code, uint8_t depth);
};

void initCore(VM &vm);

