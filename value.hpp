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

    String(VM* vm, std::string value);
};

struct List : public Object {
    Value *items;

    int capacity;
    int size;

    List(VM *vm);
    void add(Value v);
};

struct Function : public Object {
    std::vector<uint8_t> code;
    std::vector<Value> constants;

    int localCount;

    bool foreign;

    Function(VM *vm);
};

struct Value {
    union {
        double num;
        Object *object;
    } as;
    bool isObject;
};

Value newNum(double n);
Value newString(VM* vm, std::string s);
Value newList(VM *vm);
Value newFunction(VM *vm);

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

    TOKEN_FUNCTION,
    TOKEN_CLASS,

    TOKEN_COMMA,
    TOKEN_LINE,

    TOKEN_RETURN,
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
    Compiler *parent;

    std::vector<Token> input;
    int it;

    Token current;
    Token next;

    std::map<std::string, int> vars;
    std::vector<uint8_t> code;

    void consume();
    void match(TokenType type);

    void term();

    bool isAddop(TokenType type);
    bool isRelop(TokenType type);

    void ifBlock();
    void whileBlock();
    void arguments();
    void createFunction();
    void classStatement();
    void deleteStatement();
    void returnStatement();
    void statement();
    void function();
    void block();
    void expression();
    void assignment();

    void setVar(std::string name);

    void add();
    void sub();

    void factor();

    void mul();
    void div();

public:
    std::vector<Value> constants;
    std::map<std::string, int> symbolsTable;
    int varOffset;

    Compiler(VM *vm, Compiler *parent);
    ~Compiler();

    uint8_t findSymbol(std::string symbol);
    int findVar(std::string name);

    std::vector<uint8_t> compile(std::vector<Token> in);
};

struct ObjectClass {
    std::map<int, std::function<void(VM *vm, Value *args)> > symbols;
};

struct CallFrame {
    CallFrame(uint8_t *ip, std::vector<Value> constants, int memorySize) :
        ip(ip),
        constants(constants),
        memorySize(memorySize)
    {
    }

    uint8_t *ip;
    std::vector<Value> constants;
    int memorySize;
};

class VM {
    uint8_t *ip;
    int memoryOffset;
    std::vector<Value> constants;

public:
    std::vector<Value> memory;
    
    Compiler *compiler;
    ObjectClass *numClass;
    ObjectClass *strClass;
    ObjectClass *listClass;
    ObjectClass *functionClass;

    std::map<Function *, std::function<void(VM *vm, Value *args)> > functions;

    std::vector<Value> stack;
    std::vector<CallFrame> frames;

    VM();
    ~VM();

    Value pop();

    void push(Value val);

    void printStack();

    void run(std::string code);

    void callMethod(uint8_t code, uint8_t depth);
    void callFunction(uint8_t depth);

    void pushFrame();
    void popFrame();
};

void initCore(VM &vm);

