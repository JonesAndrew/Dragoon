#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "value.hpp"

enum Instruction {
    MOVB,

    JUMP,
    JUMP_BACK,
    JEQ,

    MEM,
    MEMSET,

    POP,

    ADD,
    SUB,
    MUL,

    CALL,
    CALL_FUNC,

    DEL,

    RETURN,
};

std::map<TokenType, std::string> TYPE_TO_STRING = {
    {TOKEN_NUMBER, "TOKEN_NUMBER"},
    {TOKEN_STRING, "TOKEN_STRING"},
    {TOKEN_IDENT, "TOKEN_IDENT"},
    {TOKEN_SYMBOL, "TOKEN_SYMBOL"},
    {TOKEN_SYMBOL_START, "TOKEN_SYMBOL_START"},

    {TOKEN_TRUE, "TOKEN_TRUE"},
    {TOKEN_FALSE, "TOKEN_FALSE"},

    {TOKEN_ADD, "TOKEN_ADD"},
    {TOKEN_SUB, "TOKEN_SUB"},
    {TOKEN_MUL, "TOKEN_MUL"},
    {TOKEN_DIV, "TOKEN_DIV"},

    {TOKEN_EQ, "TOKEN_EQ"},

    {TOKEN_LT, "TOKEN_LT"},
    {TOKEN_GT, "TOKEN_GT"},
    {TOKEN_LTEQ, "TOKEN_LTEQ"},
    {TOKEN_GTEQ, "TOKEN_GTEQ"},
    {TOKEN_EQEQ, "TOKEN_EQEQ"},

    {TOKEN_LPAREN, "TOKEN_LPAREN"},
    {TOKEN_RPAREN, "TOKEN_RPAREN"},
    {TOKEN_LCURLY, "TOKEN_LCURLY"},
    {TOKEN_RCURLY, "TOKEN_RCURLY"},

    {TOKEN_IF, "TOKEN_IF"},
    {TOKEN_ELSE, "TOKEN_ELSE"},

    {TOKEN_WHILE, "TOKEN_WHILE"},

    {TOKEN_FUNCTION, "TOKEN_FUNCTION"},
    {TOKEN_CLASS, "TOKEN_CLASS"},

    {TOKEN_COMMA, "TOKEN_COMMA"},
    {TOKEN_LINE, "TOKEN_LINE"},

    {TOKEN_EMPTY, "TOKEN_EMPTY"},
    {TOKEN_RETURN, "TOKEN_RETURN"},
};

std::map<char, TokenType> SINGLE_CHAR_TOKENS = {
    {'+', TOKEN_ADD},
    {'-', TOKEN_SUB},
    {'*', TOKEN_MUL},
    {'/', TOKEN_DIV},

    {'=', TOKEN_EQ},

    {'<', TOKEN_LT},

    {'(', TOKEN_LPAREN},
    {')', TOKEN_RPAREN},
    {'{', TOKEN_LCURLY},
    {'}', TOKEN_RCURLY},

    {',', TOKEN_COMMA},

    {'[', TOKEN_LBRACKET},
    {']', TOKEN_RBRACKET},

    {'\n', TOKEN_LINE},
};

std::map<std::string, TokenType> KEYWORDS = {
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},

    {"function", TOKEN_FUNCTION},
    {"class", TOKEN_CLASS},

    {"true", TOKEN_TRUE},
    {"false", TOKEN_FALSE},

    {"delete", TOKEN_DELETE},

    {"return", TOKEN_RETURN},
};

void error(std::string err) {
    std::cout << "Error: " << err << std::endl;
}

void abort(std::string err) {
    error(err);
    exit(0);
}

void expected(std::string expect) {
    abort(expect + " expected.");
}

class Tokenizer {
    std::string input;
    int it;

    char lookAhead;

    std::vector<Token> tokens;

    void getChar() {
        lookAhead = input[it++];
    }

    char peek() {
        return input[it];
    }

    void match(char c) {
        if (c == lookAhead)  {
            getChar();
            skipWhite();
        }
        else
            expected(std::string(1, c));
    }

    void twoChar(std::string combo, TokenType correct, TokenType wrong) {
        match(combo[0]);

        if (lookAhead == combo[1]) {
            match(combo[1]);
            add(correct, combo);
        } else {
            add(wrong, std::string(1, combo[0]));
        }
    }

    bool isAlpha(char c) {
        return isalpha(c);
    }

    bool isDigit(char c) {
        return isdigit(c);
    }

    bool isAlNum(char c) {
        return isAlpha(c) || isDigit(c);
    }

    bool isWhiteSpace(char c) {
        return c != '\n' && isspace(c);
    }

    void skipWhite() {
        while (isWhiteSpace(lookAhead)) {
            getChar();
        }
    }

    std::string getName() {
        std::string name;

        if (!isAlpha(lookAhead)) {
            expected("Name");
        }

        while (isAlNum(lookAhead)) {
            name += lookAhead;
            getChar();
        }

        return name;
    }

    std::string getNum() {
        std::string value;

        if (!isDigit(lookAhead))
            expected("Number");

        while (isDigit(lookAhead) || (lookAhead == '.' && isDigit(peek()))) {
            value += lookAhead;
            getChar();
        }

        skipWhite();

        return value;
    }

    std::string getString() {
        std::string value;

        if (lookAhead != '"')
            expected("\"");
        
        getChar();

        while (lookAhead != '"') {
            value += lookAhead;
            getChar();
        }

        match('"');

        return value;
    }

    void function(std::string name, bool method = true) {
        add(TOKEN_SYMBOL_START, method ? "" : name);

        match('(');

        int opens = 1;

        do {
            if (lookAhead == '(') opens++;
            else if (lookAhead == ')') {
                opens--;
                if (opens <= 0) break;
            }
            step();
        } while (true);

        add(TOKEN_SYMBOL, name);
        match(')');
    }

    void add(TokenType type, std::string value = "") {
        if (TOKEN_EMPTY == type) throw 0;

        tokens.push_back(Token(type, value));
    }

public:
    Tokenizer(std::string input) :
        input(input),
        it(0)
    {
        getChar();
        skipWhite();
    }

    void step() {
        if (isDigit(lookAhead)) {
            std::string num = getNum();
            add(TOKEN_NUMBER, num);
        } else if (lookAhead == '"') {
            std::string str = getString();
            add(TOKEN_STRING, str);
        } else if (isAlpha(lookAhead)) {
            std::string name = getName();

            if (KEYWORDS.find(name) != KEYWORDS.end()) {
                add(KEYWORDS[name]);

                if (name == "function") {
                    skipWhite();
                    if (lookAhead != '(') {
                        add(TOKEN_IDENT, getName());
                    }
                }
            } else if (lookAhead == '(') {
                // Function
                function(name, false);
            } else {
                add(TOKEN_IDENT, name);
            }

            skipWhite();
        } else if (lookAhead == '.') {
            // Method
            getChar();
            
            std::string name = getName();
            function(name);
        } else {
            if (lookAhead == '<') {
                twoChar("<=", TOKEN_LTEQ, TOKEN_LT);
            } else if (lookAhead == '>') {
                twoChar(">=", TOKEN_GTEQ, TOKEN_GT);
            } else if (lookAhead == '=') {
                twoChar("==", TOKEN_EQEQ, TOKEN_EQ);
            } else if (SINGLE_CHAR_TOKENS.find(lookAhead) != SINGLE_CHAR_TOKENS.end()) {
                add(SINGLE_CHAR_TOKENS[lookAhead]);

                getChar();
                skipWhite();
            } else {
                expected("SINGLE_CHAR_TOKENS");
            }
        }
    }

    std::vector<Token> tokenize() {
        while (it <= input.size()) {
            step();
        }

        return tokens;
    }
};

void Compiler::consume() {
    if (it >= input.size()) {
        current = Token(TOKEN_EMPTY);
    } else {
        current = input[it];
    }
    it++;
    if (it >= input.size()) {
        next = Token(TOKEN_EMPTY);
    } else {
        next = input[it];
    }
}

void Compiler::match(TokenType type) {
    if (type == current.type)  {
        consume();
    } else {
        expected("Got " + TYPE_TO_STRING[current.type] + ", type " + TYPE_TO_STRING[type]);
    }
}

void Compiler::term() {
    factor();
    while (current.type == TOKEN_MUL || current.type == TOKEN_DIV) {
        switch(current.type) {
            case TOKEN_MUL:
                mul();
                break;
            case TOKEN_DIV:
                div();
                break;
            default:
                expected("Mulop");
                break;
        }
    }
}

bool Compiler::isAddop(TokenType type) {
    return type == TOKEN_ADD || type == TOKEN_SUB;
}

bool Compiler::isRelop(TokenType type) {
    return type == TOKEN_LT || type == TOKEN_LTEQ ||
           type == TOKEN_GT || type == TOKEN_GTEQ ||
           type == TOKEN_EQ;
}

void Compiler::ifBlock() {
    match(TOKEN_IF);
    match(TOKEN_LPAREN);
    expression();
    match(TOKEN_RPAREN);

    code.push_back(JEQ);
    int start = code.size();
    code.push_back(0);

    block();

    if (current.type == TOKEN_ELSE) {
        match(TOKEN_ELSE);

        code.push_back(JUMP);
        int elseStart = code.size();
        code.push_back(0);

        code[start] = code.size() - start;

        if (current.type == TOKEN_IF) {
            ifBlock();
        } else {
            block();
        }

        code[elseStart] = code.size() - elseStart;
    } else {
        code[start] = code.size() - start;
    }
}

void Compiler::whileBlock() {
    int ifStart = code.size();

    match(TOKEN_WHILE);
    match(TOKEN_LPAREN);
    expression();
    match(TOKEN_RPAREN);

    code.push_back(JEQ);
    int start = code.size();
    code.push_back(0);

    block();

    code.push_back(JUMP_BACK);
    code.push_back(code.size() - ifStart);

    code[start] = code.size() - start;
}

void Compiler::statement() {
    if (current.type == TOKEN_IF) {
        ifBlock();
    } else if (current.type == TOKEN_WHILE) {
        whileBlock();
    } else if (next.type == TOKEN_EQ) {
        assignment();
    } else if (current.type == TOKEN_DELETE) {
        deleteStatement();
    } else if (current.type == TOKEN_LINE) {
        consume();
    } else if (current.type == TOKEN_RETURN) {
        returnStatement();
    } else if (current.type == TOKEN_CLASS) {
        classStatement();
    } else if (current.type == TOKEN_FUNCTION) {
        createFunction();
    } else {
        expression();
    }
}

void Compiler::arguments() {
    match(TOKEN_LPAREN);

    if (current.type == TOKEN_RPAREN) {
        consume();
        return;
    }

    setVar(current.value);
    consume();

    while (current.type != TOKEN_RPAREN) {
        match(TOKEN_COMMA);
        setVar(current.value);
        consume();
    }
    consume();
}

void Compiler::createFunction() {
    match(TOKEN_FUNCTION);

    std::string fnName;
    if (current.type == TOKEN_IDENT) {
        fnName = current.value;
        match(TOKEN_IDENT);
    }

    auto temp = code;
    Value func = newFunction(vm);

    code.push_back(MOVB);
    code.push_back(constants.size());
    constants.push_back(func);

    Compiler fnCompiler(vm, this);

    fnCompiler.arguments();
    fnCompiler.block();
    fnCompiler.code.push_back(MOVB);
    fnCompiler.code.push_back(constants.size());
    fnCompiler.constants.push_back(newNum(0));
    fnCompiler.code.push_back(RETURN);

    if (fnName != "") {
        setVar(fnName);
    }

    Function *fn = AS(func, Function);
    fn->code = std::vector<uint8_t>(fnCompiler.code);
    fn->constants = fnCompiler.constants;
    fn->localCount = fnCompiler.varOffset;

    it = fnCompiler.it;
    current = fnCompiler.current;
    next = fnCompiler.next;
}

uint8_t Compiler::findSymbol(std::string symbol) {
    auto &table = vm->compiler->symbolsTable;

    auto it = table.find(symbol);

    if (it != table.end()) {
        return it->second;
    } else {
        int size = table.size();
        table[symbol] = size;
        return size;
    }
}

void Compiler::function() {
    bool method = current.value == "";

    if (!method) {
        code.push_back(MEM);

        int var = findVar(current.value);
        if (var == -1) {
            abort(current.value + " used before init.");
        }

        code.push_back(var);
    }

    match(TOKEN_SYMBOL_START);
    uint8_t depth = 0;

    while (current.type != TOKEN_SYMBOL) {
        expression();
        if (current.type == TOKEN_COMMA) {
            match(TOKEN_COMMA);
        }
        depth++;
    }

    if (method) {
        code.push_back(CALL);
        code.push_back(findSymbol(current.value));
        code.push_back(depth);
    } else {
        code.push_back(CALL_FUNC);
        code.push_back(depth);
    }

    match(TOKEN_SYMBOL);
}

void Compiler::block() {
    match(TOKEN_LCURLY);
    while (current.type != TOKEN_RCURLY) {
        statement();
    }
    match(TOKEN_RCURLY);
}

void Compiler::expression() {
    if (isAddop(current.type)) {
        code.push_back(MOVB);
        code.push_back(constants.size());
        constants.push_back(newNum(0));
    } else {
        term();
    }

    while (isAddop(current.type)) {
        switch(current.type) {
            case TOKEN_ADD:
                add();
                break;
            case TOKEN_SUB:
                sub();
                break;
            default:
                expected("Addop");
                break;
        }
    }

    if (isRelop(current.type)) {
        int symbol = findSymbol(current.value);
        consume();
        expression();

        code.push_back(CALL);
        code.push_back(symbol);
        code.push_back(1);
    }
}

int Compiler::findVar(std::string name) {
    auto result = vars.find(name);
    if (vars.find(name) == vars.end()) {
        // if (parent != nullptr) {
        //     return parent->findVar(name);
        // }
        
        return -1;
    }

    return result->second;
}

void Compiler::assignment() {
    std::string name = current.value;
    consume();

    match(TOKEN_EQ);
    expression();

    setVar(name);
}

void Compiler::setVar(std::string name) {
    int var = findVar(name);
    if (var == -1) {
        var = varOffset;
        vars[name] = var;
        varOffset++;
    }

    code.push_back(MEMSET);
    code.push_back(var);
}

void Compiler::deleteStatement() {
    match(TOKEN_DELETE);

    factor();

    code.push_back(DEL);
}

void Compiler::returnStatement() {
    match(TOKEN_RETURN);

    if (current.type == TOKEN_LINE) {
        code.push_back(MOVB);
        code.push_back(constants.size());
        constants.push_back(newNum(0));
    } else {
        expression();
    }

    code.push_back(RETURN);
}

void Compiler::classStatement() {
    match(TOKEN_CLASS);

    fnName = current.value;
    match(TOKEN_IDENT);
}

void Compiler::add() {
    match(TOKEN_ADD);
    term();

    code.push_back(CALL);
    code.push_back(findSymbol("+"));
    code.push_back(1);
}

void Compiler::sub() {
    match(TOKEN_SUB);
    term();

    code.push_back(CALL);
    code.push_back(findSymbol("-"));
    code.push_back(1);
}

void Compiler::factor() {
    if (current.type == TOKEN_LPAREN) {
        match(TOKEN_LPAREN);
        expression();
        match(TOKEN_RPAREN);
    } else if (current.type == TOKEN_IDENT) {
        code.push_back(MEM);

        int var = findVar(current.value);
        if (var == -1) {
            abort(current.value + " used before init.");
        }

        code.push_back(var);
        consume();
    } else if (current.type == TOKEN_TRUE) {
        code.push_back(MOVB);
        code.push_back(constants.size());
        constants.push_back(newNum(1));

        consume();
    } else if (current.type == TOKEN_FALSE) {
        code.push_back(MOVB);
        code.push_back(constants.size());
        constants.push_back(newNum(0));

        consume();
    } else if (current.type == TOKEN_NUMBER) {
        code.push_back(MOVB);
        code.push_back(constants.size());
        constants.push_back(newNum(std::stof(current.value)));

        consume();
    } else if (current.type == TOKEN_STRING) {
        code.push_back(MOVB);
        code.push_back(constants.size());
        constants.push_back(newString(vm, current.value));

        consume();
    } else if (current.type == TOKEN_LBRACKET) {
        consume();

        code.push_back(MOVB);
        code.push_back(constants.size());
        constants.push_back(newList(vm));

        match(TOKEN_RBRACKET);
    } else if (current.type == TOKEN_SYMBOL_START) {
        function();
        return;
    } else if (current.type == TOKEN_FUNCTION) {
        createFunction();
    } else {
        abort("Unexpected token " + TYPE_TO_STRING[current.type] + ".");
    }

    if (current.type == TOKEN_SYMBOL_START) {
        function();
    }
}

void Compiler::mul() {
    match(TOKEN_MUL);
    factor();

    code.push_back(CALL);
    code.push_back(findSymbol("*"));
    code.push_back(1);
}

void Compiler::div() {
    match(TOKEN_DIV);
    factor();

    code.push_back(CALL);
    code.push_back(findSymbol("/"));
    code.push_back(1);
}

Compiler::Compiler(VM *vm, Compiler *parent) :
    vm(vm),
    parent(parent),
    it(0),
    current(TOKEN_EMPTY),
    next(TOKEN_EMPTY),
    varOffset(0)
{
    code = std::vector<uint8_t>();

    if (parent != nullptr) {
        input = parent->input;
        it = parent->it;
        current = parent->current;
        next = parent->next;
    }
}

Compiler::~Compiler() {
}

std::vector<uint8_t> Compiler::compile(std::vector<Token> in) {
    code = std::vector<uint8_t>();
    input = in;
    it = 0;

    consume();
    while (it <= input.size()) {
        statement();
    }
    code.push_back(RETURN);

    return code;
}

VM::VM() {
    memoryOffset = 0;

    numClass = new ObjectClass();
    strClass = new ObjectClass();
    listClass = new ObjectClass();
    functionClass = new ObjectClass();

    compiler = new Compiler(this, nullptr);

    run(
        "function print() {}"
    );

    initCore(*this);
}

VM::~VM() {
    delete numClass;
    delete strClass;
    delete listClass;
    delete functionClass;

    delete compiler;
}

Value VM::pop() {
    Value v = stack.back();
    stack.pop_back();
    return v;
}

void VM::push(Value val) {
    stack.push_back(val);
}

std::string valueToStr(VM *vm, Value v) {
    if (!v.isObject) return std::to_string(v.as.num);

    if (v.as.object->classObject == vm->listClass) {
        std::string final = "[";
        
        List *list = AS(v, List);

        for (int i = 0; i < list->size; i++) {
            final += valueToStr(vm, list->items[i]);

            if (i != list->size - 1)
                final += ", ";
        }

        return final + "]";
    } else if (v.as.object->classObject == vm->functionClass) {
        return "function";
    }

    return AS(v, String)->value;
}

void VM::printStack() {
    for (auto v : stack) {
        printf("stack: %s\n", valueToStr(this, v).c_str());
    }
}

void VM::run(std::string code) {
    Tokenizer tz(code);
    auto tokens = tz.tokenize();

    auto instructions = compiler->compile(tokens);
    ip = &instructions.front();
    constants = compiler->constants;
    uint8_t dif = 0;

    for (int i=memory.size(); i<compiler->varOffset; i++) {
        memory.push_back({});
    }

    while (true) {
        uint8_t c = *ip++;
        switch (c) {
            case MOVB:
                push(constants[*ip++]);
                break;

            case JUMP:
                dif = *ip;
                ip += dif;
                break;

            case JUMP_BACK:
                dif = *ip;
                ip -= dif;
                break;

            case JEQ:
                if (pop().as.num == 0) {
                    dif = *ip;
                    ip += dif;
                } else {
                    ip++;
                }
                break;

            case MEM:
                push(memory[*ip + memoryOffset]);
                ip++;
                break;

            case MEMSET:
                memory[*ip + memoryOffset] = pop();
                ip++;
                break;

            case POP:
                pop();
                break;

            case CALL:
                callMethod(*ip, *(ip + 1));
                ip += 2;
                break;

            case CALL_FUNC:
                callFunction(*ip++);
                break;

            case DEL:
                delete pop().as.object;
                break;

            case RETURN:
                if (frames.empty())
                    return;
                else
                    popFrame();
                break;
        }
    }
}

void VM::callMethod(uint8_t code, uint8_t depth) {
    Value *args = &(stack.end()[- depth - 1]);
    for (int i = 0; i < depth + 1; i++)
        stack.pop_back();

    std::map<int, std::function<void(VM *vm, Value *args)> > *symbols;

    if (!args[0].isObject)
        symbols = &numClass->symbols;
    else
        symbols = &args[0].as.object->classObject->symbols;

    auto it = symbols->find(code);
    if (it != symbols->end()) {
        it->second(this, args);
    } else {
        std::string symbol = "";
        for (auto it : compiler->symbolsTable) {
            if (it.second == code) {
                symbol = it.first;
                break;
            } 
        }

        printf("Missing method %s on %s\n", symbol.c_str(), valueToStr(this, args[0]).c_str());
        exit(0);
    }
}

void VM::callFunction(uint8_t depth) {
    Value function = stack.end()[- depth - 1];

    Function *fn = AS(function, Function);

    if (!fn->foreign) {
        pushFrame();

        stack.erase(stack.end() - depth - 1);

        ip = &fn->code.front();
        constants = fn->constants;
        memoryOffset = memory.size();

        for (int i=0; i<fn->localCount; i++) {
            memory.push_back({});
        }
    } else {
        Value *args = &(stack.end()[- depth - 1]);
        for (int i = 0; i < depth + 1; i++)
            stack.pop_back();

        functions[fn](this, args);
    }
}

void VM::pushFrame() {
    frames.push_back({ip, constants, memoryOffset});
}

void VM::popFrame() {
    int remove = memory.size() - memoryOffset;
    for (int i=0; i<remove; i++) {
        memory.pop_back();
    }

    auto &frame = frames.back();
    ip = frame.ip;
    constants = frame.constants;
    memoryOffset = frame.memorySize;

    frames.pop_back();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Pass filename as argument.\n");
        return 0;
    }

    std::ifstream t(argv[1]);
    std::string code((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());

    VM vm;
    vm.run(code);

    return 0;
}
