#pragma once
#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H

#include <any>
#include <set>
#include <map>
#include <vector>
#include <string>
#include "int2048.h"
#include "Python3ParserBaseVisitor.h"

// Structure to hold argument information for function calls
struct Argument {
  std::string name;
  std::any default_value;
};

// Structure to hold function information
struct Function {
  std::vector<Argument> parameters;
  Python3Parser::SuiteContext* body;
};

class EvalVisitor : public Python3ParserBaseVisitor {
private:
  // Stack of variable scopes
  std::vector<std::map<std::string, std::any>> variables;

  // Map of function names to their definitions
  std::map<std::string, Function> functions;

  // System functions
  std::set<std::string> systemFunctions = {"print", "int", "float", "str", "bool"};
  std::any callSystemFunction(const std::string &name, const std::vector<std::any> &args);

  // Find the value of a variable
  std::any getVariable(const std::string &name);

  // Set the value of a variable
  void setVariable(const std::string &name, const std::any &value);

  // Perform operations include + - * / // % > < >= <= == !=
  // Throws runtime_error for unsupported operand types
  std::any operate(const std::string &op, std::any left, std::any right);

  // Type conversion helpers
  std::any to_int(const std::any &value);
  std::any to_bool(const std::any &value);
  std::any to_double(const std::any &value);
  std::any to_string(const std::any &value);

  // Print function
  std::any print(const std::vector<std::any> &args);

public:
  // Constructor for EvalVisitor
  // Initializes the variable scope stack with a global scope.
  EvalVisitor();

  // Visit the parse tree nodes produced by Python3Parser and evaluate the Python code.
  // If a node throws a runtime_error, it indicates an error during evaluation.
  std::any visitFile_input(Python3Parser::File_inputContext *ctx) override;

  // Stores the function definition in the functions map.
  std::any visitFuncdef(Python3Parser::FuncdefContext *ctx) override;

  // Get the parameters of a function definition.
  std::any visitParameters(Python3Parser::ParametersContext *ctx) override;

  // Get the typed argument list of a function definition.
  // If an argument has a default value, it is stored in the Argument struct.
  // Otherwise, the default_value is set to std::any().
  std::any visitTypedargslist(Python3Parser::TypedargslistContext *ctx) override;

  // Get the name of a typed function parameter.
  std::any visitTfpdef(Python3Parser::TfpdefContext *ctx) override;

  // Visit statements and evaluate them.
  // Throws a runtime_error if the statement is neither a simple nor compound statement.
  std::any visitStmt(Python3Parser::StmtContext *ctx) override;

  // Visit simple statements.
  std::any visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) override;

  // Visit small statements.
  // Throws a runtime_error if the small statement is neither an expression nor flow statement.
  std::any visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) override;

  // Visit expression statements.
  std::any visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) override;

  // Visit augmented assignment statements.
  // Returns the operation as a string: +=, -=, *=, /=, //=, %=.
  std::any visitAugassign(Python3Parser::AugassignContext *ctx) override;

  // Visit flow statements.
  std::any visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) override;

  // Visit break statements.
  std::any visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) override;

  // Visit continue statements.
  std::any visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) override;

  // Visit return statements and return the return value.
  std::any visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) override;

  // Visit compound statements.
  std::any visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) override;

  // Visit if statements.
  std::any visitIf_stmt(Python3Parser::If_stmtContext *ctx) override;

  // Visit while statements.
  std::any visitWhile_stmt(Python3Parser::While_stmtContext *ctx) override;

  // Visit suite statements.
  std::any visitSuite(Python3Parser::SuiteContext *ctx) override;

  // Visit test expressions.
  std::any visitTest(Python3Parser::TestContext *ctx) override;

  // Visit or_test expressions.
  // If any of the and_test children is true, returns true immediately (short-circuit).
  std::any visitOr_test(Python3Parser::Or_testContext *ctx) override;

  // Visit and_test expressions.
  // If any of the not_test children is false, returns false immediately (short-circuit).
  std::any visitAnd_test(Python3Parser::And_testContext *ctx) override;

  // Visit not_test expressions.
  std::any visitNot_test(Python3Parser::Not_testContext *ctx) override;

  // Visit comparison expressions.
  // If any of the comparison children is false, returns false immediately (short-circuit).
  std::any visitComparison(Python3Parser::ComparisonContext *ctx) override;

  // Visit comparison operators.
  // Returns the operator as a string: <, >, ==, >=, <=, !=.
  std::any visitComp_op(Python3Parser::Comp_opContext *ctx) override;

  // Visit arithmetic expressions.
  std::any visitArith_expr(Python3Parser::Arith_exprContext *ctx) override;

  // Visit addition and subtraction operators.
  // Returns the operator as a string: + or -.
  std::any visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) override;

  // Visit term expressions.
  std::any visitTerm(Python3Parser::TermContext *ctx) override;

  // Visit multiplication, division, floor division, and modulus operators.
  // Returns the operator as a string: *, /, //, or %.
  std::any visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) override;

  // Visit factor expressions.
  // Returns a int value for + or - operators applied to a bool type.
  std::any visitFactor(Python3Parser::FactorContext *ctx) override;

  // Visit atom expressions.
  std::any visitAtom_expr(Python3Parser::Atom_exprContext *ctx) override;

  // Visit trailers, including function calls.
  std::any visitTrailer(Python3Parser::TrailerContext *ctx) override;

  // Visit atom nodes.
  std::any visitAtom(Python3Parser::AtomContext *ctx) override;

  // Visit Format string nodes.
  std::any visitFormat_string(Python3Parser::Format_stringContext *ctx) override;

  // Visit testlist nodes.
  std::any visitTestlist(Python3Parser::TestlistContext *ctx) override;

  // Visit argument list nodes.
  std::any visitArglist(Python3Parser::ArglistContext *ctx) override;

  // Visit argument nodes.
  std::any visitArgument(Python3Parser::ArgumentContext *ctx) override;
};


#endif//PYTHON_INTERPRETER_EVALVISITOR_H
