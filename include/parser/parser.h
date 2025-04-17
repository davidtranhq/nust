#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <stdexcept>

namespace nust {

// Forward declarations
class ASTNode;
class Program;
class FunctionDecl;
class Stmt;
class Expr;
class Type;

// Precedence levels for binary operators
enum class Precedence {
    None,
    Assignment,  // =
    Or,         // ||
    And,        // &&
    Equality,   // == !=
    Comparison, // < > <= >=
    Term,       // + -
    Factor,     // * /
    Unary,      // ! -
    Call,       // . () []
    Primary
};

// AST Node types
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> items;
    Program(std::vector<std::unique_ptr<ASTNode>> items) : items(std::move(items)) {}
};

class FunctionDecl : public ASTNode {
public:
    struct Param {
        bool is_mut;
        std::string name;
        std::unique_ptr<Type> type;
    };
    
    std::string name;
    std::vector<Param> params;
    std::unique_ptr<Stmt> body;
    
    FunctionDecl(std::string name, std::vector<Param> params, std::unique_ptr<Stmt> body)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}
};

class Stmt : public ASTNode {
public:
    virtual ~Stmt() = default;
};

class LetStmt : public Stmt {
public:
    bool is_mut;
    std::string name;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expr> init;
    
    LetStmt(bool is_mut, std::string name, std::unique_ptr<Type> type, std::unique_ptr<Expr> init)
        : is_mut(is_mut), name(std::move(name)), type(std::move(type)), init(std::move(init)) {}
};

class ExprStmt : public Stmt {
public:
    std::unique_ptr<Expr> expr;
    ExprStmt(std::unique_ptr<Expr> expr) : expr(std::move(expr)) {}
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> then_branch;
    std::unique_ptr<Stmt> else_branch;
    
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> then_branch, std::unique_ptr<Stmt> else_branch)
        : condition(std::move(condition)), then_branch(std::move(then_branch)), else_branch(std::move(else_branch)) {}
};

class WhileStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
    
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}
};

class BlockStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements) : statements(std::move(statements)) {}
};

class Expr : public ASTNode {
public:
    virtual ~Expr() = default;
};

class IntLiteral : public Expr {
public:
    int value;
    IntLiteral(int value) : value(value) {}
};

class BoolLiteral : public Expr {
public:
    bool value;
    BoolLiteral(bool value) : value(value) {}
};

class StringLiteral : public Expr {
public:
    std::string value;
    StringLiteral(std::string value) : value(std::move(value)) {}
};

class Identifier : public Expr {
public:
    std::string name;
    Identifier(std::string name) : name(std::move(name)) {}
};

class BinaryExpr : public Expr {
public:
    enum class Op {
        Add, Sub, Mul, Div,
        Eq, Ne, Lt, Gt, Le, Ge
    };
    Op op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    
    BinaryExpr(Op op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

class UnaryExpr : public Expr {
public:
    enum class Op { Neg, Not };
    Op op;
    std::unique_ptr<Expr> expr;
    
    UnaryExpr(Op op, std::unique_ptr<Expr> expr) : op(op), expr(std::move(expr)) {}
};

class BorrowExpr : public Expr {
public:
    bool is_mut;
    std::unique_ptr<Expr> expr;
    
    BorrowExpr(bool is_mut, std::unique_ptr<Expr> expr) : is_mut(is_mut), expr(std::move(expr)) {}
};

class CallExpr : public Expr {
public:
    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> args;
    
    CallExpr(std::unique_ptr<Expr> callee, std::vector<std::unique_ptr<Expr>> args)
        : callee(std::move(callee)), args(std::move(args)) {}
};

class Type {
public:
    enum class Kind {
        I32, Bool, Str,
        Ref, MutRef
    };
    Kind kind;
    std::unique_ptr<Type> base_type; // For Ref and MutRef
    
    Type(Kind kind) : kind(kind) {}
    Type(Kind kind, std::unique_ptr<Type> base_type) : kind(kind), base_type(std::move(base_type)) {}
};

class Parser {
public:
    Parser(std::string source);
    std::unique_ptr<Program> parse();

private:
    // Helper functions
    bool match(const std::string& expected);
    bool peek(const std::string& expected);
    void expect(const std::string& expected);
    std::string consume_identifier();
    int consume_integer();
    std::string consume_string();
    void skip_whitespace();
    bool at_end();
    void error(const std::string& message);
    void synchronize();
    
    // Parsing functions
    std::unique_ptr<FunctionDecl> parse_function();
    std::vector<FunctionDecl::Param> parse_params();
    std::unique_ptr<Type> parse_type();
    std::unique_ptr<Stmt> parse_statement();
    std::unique_ptr<LetStmt> parse_let();
    std::unique_ptr<IfStmt> parse_if();
    std::unique_ptr<WhileStmt> parse_while();
    std::unique_ptr<BlockStmt> parse_block();
    std::unique_ptr<Expr> parse_expr();
    std::unique_ptr<Expr> parse_equality();
    std::unique_ptr<Expr> parse_comparison();
    std::unique_ptr<Expr> parse_term();
    std::unique_ptr<Expr> parse_factor();
    std::unique_ptr<Expr> parse_unary();
    std::unique_ptr<Expr> parse_call();
    std::unique_ptr<Expr> parse_primary();
    
    std::string source;
    size_t pos = 0;
};

} // namespace nust 