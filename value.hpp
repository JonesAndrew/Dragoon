#pragma once

#include <map>
#include <string>
#include <vector>
#include <stack>

struct Value;

struct Object {
    int objectType;

    std::vector<Value> fields;
};

struct Value {
    union {
        double num;
        Object *object;
    } as;
    bool isObject;
};

inline Value numValue(double n) {
    Value v;
    v.as.num = n;
    return v;
}

enum TokenType {
    TOKEN_EMPTY,

    TOKEN_NUMBER,
    TOKEN_IDENT,
    TOKEN_SYMBOL,
    TOKEN_SYMBOL_START,

    TOKEN_TRUE,
    TOKEN_FALSE,

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

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LCURLY,
    TOKEN_RCURLY,

    TOKEN_IF,
    TOKEN_ELSE,

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

    Compiler();

    std::vector<uint8_t> compile(std::vector<Token> in);
};

class VM;

struct ObjectClass {
    std::map<int, std::function<void(VM *vm)> > symbols;
};

class VM {
    std::stack<Value> stack;
    std::vector<Value> memory;

public:
    Compiler *compiler;
    ObjectClass *numClass;

    VM();

    Value pop();

    void push(Value val);

    void printStack();

    void run(std::vector<Token> input);
};

void initCore(VM &vm);

