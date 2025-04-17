#include "parser/parser.h"
#include <cctype>
#include <sstream>
#include <stdexcept>

namespace nust {

Parser::Parser(std::string source) : source(std::move(source)) {}

std::unique_ptr<Program> Parser::parse() {
    std::vector<std::unique_ptr<ASTNode>> items;
    
    skip_whitespace();
    while (!at_end()) {
        try {
            items.push_back(parse_function());
        } catch (const std::runtime_error& e) {
            synchronize();
        }
        skip_whitespace();
    }
    
    return std::make_unique<Program>(std::move(items));
}

std::unique_ptr<FunctionDecl> Parser::parse_function() {
    expect("fn");
    skip_whitespace();
    
    std::string name = consume_identifier();
    skip_whitespace();
    
    expect("(");
    auto params = parse_params();
    expect(")");
    skip_whitespace();
    
    auto body = parse_block();
    
    return std::make_unique<FunctionDecl>(std::move(name), std::move(params), std::move(body));
}

std::vector<FunctionDecl::Param> Parser::parse_params() {
    std::vector<FunctionDecl::Param> params;
    
    skip_whitespace();
    if (peek(")")) return params;
    
    do {
        skip_whitespace();
        bool is_mut = match("mut");
        if (is_mut) skip_whitespace();
        
        std::string name = consume_identifier();
        skip_whitespace();
        
        expect(":");
        skip_whitespace();
        
        auto type = parse_type();
        
        params.push_back(FunctionDecl::Param{is_mut, std::move(name), std::move(type)});
        skip_whitespace();
    } while (match(","));
    
    return params;
}

std::unique_ptr<Type> Parser::parse_type() {
    if (match("&")) {
        bool is_mut = match("mut");
        if (is_mut) skip_whitespace();
        auto inner = parse_type();
        return std::make_unique<Type>(is_mut ? Type::Kind::MutRef : Type::Kind::Ref, std::move(inner));
    }
    
    if (match("i32")) return std::make_unique<Type>(Type::Kind::I32);
    if (match("bool")) return std::make_unique<Type>(Type::Kind::Bool);
    if (match("str")) return std::make_unique<Type>(Type::Kind::Str);
    
    error("Expected type");
    return nullptr;
}

std::unique_ptr<Stmt> Parser::parse_statement() {
    skip_whitespace();
    
    if (match("let")) return parse_let();
    if (match("if")) return parse_if();
    if (match("while")) return parse_while();
    if (match("{")) return parse_block();
    
    // Expression statement
    auto expr = parse_expr();
    skip_whitespace();
    expect(";");
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<LetStmt> Parser::parse_let() {
    skip_whitespace();
    bool is_mut = match("mut");
    if (is_mut) skip_whitespace();
    
    std::string name = consume_identifier();
    skip_whitespace();
    
    expect(":");
    skip_whitespace();
    
    auto type = parse_type();
    skip_whitespace();
    
    expect("=");
    skip_whitespace();
    
    auto init = parse_expr();
    skip_whitespace();
    expect(";");
    
    return std::make_unique<LetStmt>(is_mut, std::move(name), std::move(type), std::move(init));
}

std::unique_ptr<IfStmt> Parser::parse_if() {
    skip_whitespace();
    auto condition = parse_expr();
    skip_whitespace();
    
    auto then_branch = parse_block();
    skip_whitespace();
    
    std::unique_ptr<Stmt> else_branch;
    if (match("else")) {
        skip_whitespace();
        if (match("if")) {
            else_branch = parse_if();
        } else {
            else_branch = parse_block();
        }
    }
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<WhileStmt> Parser::parse_while() {
    skip_whitespace();
    auto condition = parse_expr();
    skip_whitespace();
    
    auto body = parse_block();
    
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<BlockStmt> Parser::parse_block() {
    expect("{");
    std::vector<std::unique_ptr<Stmt>> statements;
    
    skip_whitespace();
    while (!at_end() && !peek("}")) {
        statements.push_back(parse_statement());
        skip_whitespace();
    }
    
    expect("}");
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Expr> Parser::parse_expr() {
    return parse_equality();
}

std::unique_ptr<Expr> Parser::parse_equality() {
    auto expr = parse_comparison();
    
    while (true) {
        skip_whitespace();
        if (match("==")) {
            skip_whitespace();
            auto right = parse_comparison();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Eq, std::move(expr), std::move(right));
        } else if (match("!=")) {
            skip_whitespace();
            auto right = parse_comparison();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Ne, std::move(expr), std::move(right));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::parse_comparison() {
    auto expr = parse_term();
    
    while (true) {
        skip_whitespace();
        if (match("<")) {
            skip_whitespace();
            auto right = parse_term();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Lt, std::move(expr), std::move(right));
        } else if (match("<=")) {
            skip_whitespace();
            auto right = parse_term();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Le, std::move(expr), std::move(right));
        } else if (match(">")) {
            skip_whitespace();
            auto right = parse_term();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Gt, std::move(expr), std::move(right));
        } else if (match(">=")) {
            skip_whitespace();
            auto right = parse_term();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Ge, std::move(expr), std::move(right));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::parse_term() {
    auto expr = parse_factor();
    
    while (true) {
        skip_whitespace();
        if (match("+")) {
            skip_whitespace();
            auto right = parse_factor();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Add, std::move(expr), std::move(right));
        } else if (match("-")) {
            skip_whitespace();
            auto right = parse_factor();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Sub, std::move(expr), std::move(right));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::parse_factor() {
    auto expr = parse_unary();
    
    while (true) {
        skip_whitespace();
        if (match("*")) {
            skip_whitespace();
            auto right = parse_unary();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Mul, std::move(expr), std::move(right));
        } else if (match("/")) {
            skip_whitespace();
            auto right = parse_unary();
            expr = std::make_unique<BinaryExpr>(BinaryExpr::Op::Div, std::move(expr), std::move(right));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::parse_unary() {
    skip_whitespace();
    
    if (match("-")) {
        auto operand = parse_unary();
        return std::make_unique<UnaryExpr>(UnaryExpr::Op::Neg, std::move(operand));
    }
    
    if (match("!")) {
        auto operand = parse_unary();
        return std::make_unique<UnaryExpr>(UnaryExpr::Op::Not, std::move(operand));
    }
    
    if (match("&")) {
        bool is_mut = match("mut");
        if (is_mut) skip_whitespace();
        auto expr = parse_unary();
        return std::make_unique<BorrowExpr>(is_mut, std::move(expr));
    }
    
    return parse_call();
}

std::unique_ptr<Expr> Parser::parse_call() {
    auto expr = parse_primary();
    
    while (true) {
        skip_whitespace();
        if (match("(")) {
            std::vector<std::unique_ptr<Expr>> args;
            
            skip_whitespace();
            if (!peek(")")) {
                do {
                    args.push_back(parse_expr());
                    skip_whitespace();
                } while (match(","));
            }
            
            expect(")");
            expr = std::make_unique<CallExpr>(std::move(expr), std::move(args));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::parse_primary() {
    skip_whitespace();
    
    if (std::isdigit(source[pos])) {
        return std::make_unique<IntLiteral>(consume_integer());
    }
    
    if (match("true")) {
        return std::make_unique<BoolLiteral>(true);
    }
    
    if (match("false")) {
        return std::make_unique<BoolLiteral>(false);
    }
    
    if (source[pos] == '"') {
        return std::make_unique<StringLiteral>(consume_string());
    }
    
    if (std::isalpha(source[pos]) || source[pos] == '_') {
        return std::make_unique<Identifier>(consume_identifier());
    }
    
    if (match("(")) {
        auto expr = parse_expr();
        expect(")");
        return expr;
    }
    
    error("Expected expression");
    return nullptr;
}

bool Parser::match(const std::string& expected) {
    if (source.compare(pos, expected.length(), expected) == 0) {
        pos += expected.length();
        return true;
    }
    return false;
}

bool Parser::peek(const std::string& expected) {
    return source.compare(pos, expected.length(), expected) == 0;
}

void Parser::expect(const std::string& expected) {
    if (!match(expected)) {
        std::stringstream ss;
        ss << "Expected '" << expected << "'";
        error(ss.str());
    }
}

std::string Parser::consume_identifier() {
    size_t start = pos;
    
    if (!std::isalpha(source[pos]) && source[pos] != '_') {
        error("Expected identifier");
    }
    
    while (pos < source.length() && 
           (std::isalnum(source[pos]) || source[pos] == '_')) {
        pos++;
    }
    
    return source.substr(start, pos - start);
}

int Parser::consume_integer() {
    size_t start = pos;
    
    while (pos < source.length() && std::isdigit(source[pos])) {
        pos++;
    }
    
    return std::stoi(source.substr(start, pos - start));
}

std::string Parser::consume_string() {
    if (source[pos] != '"') {
        error("Expected string");
    }
    pos++; // Skip opening quote
    
    size_t start = pos;
    while (pos < source.length() && source[pos] != '"') {
        if (source[pos] == '\\') {
            pos++; // Skip escape character
            if (pos >= source.length()) {
                error("Unterminated string");
            }
        }
        pos++;
    }
    
    if (pos >= source.length()) {
        error("Unterminated string");
    }
    
    std::string value = source.substr(start, pos - start);
    pos++; // Skip closing quote
    return value;
}

void Parser::skip_whitespace() {
    while (pos < source.length() && 
           (std::isspace(source[pos]) || source[pos] == '\n' || source[pos] == '\r')) {
        pos++;
    }
}

bool Parser::at_end() {
    return pos >= source.length();
}

void Parser::error(const std::string& message) {
    std::stringstream ss;
    ss << "Parse error: " << message;
    throw std::runtime_error(ss.str());
}

void Parser::synchronize() {
    while (!at_end()) {
        if (source[pos] == ';') {
            pos++;
            return;
        }
        
        static const std::vector<std::string> keywords = {
            "fn", "let", "if", "else", "while"
        };
        
        for (const auto& keyword : keywords) {
            if (peek(keyword)) {
                return;
            }
        }
        
        pos++;
    }
}

} // namespace nust