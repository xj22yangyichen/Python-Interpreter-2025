#include "Evalvisitor.h"
#include <iomanip>
#include <stdlib.h>
#include <typeinfo>
#include <iostream>
#include <stdexcept>
#include <algorithm>

std::any EvalVisitor::to_int(const std::any &value) {
  sjtu::int2048 ret;
  if (auto val = std::any_cast<sjtu::int2048>(&value)) {
    ret = *val;
  }
  if (auto val = std::any_cast<double>(&value)) {
    ret = static_cast<int>(*val);
  }
  if (auto val = std::any_cast<std::string>(&value)) {
    ret.read(*val);
  }
  if (auto val = std::any_cast<bool>(&value)) {
    ret = *val ? 1 : 0;
  }
  return std::any(ret);
}

std::any EvalVisitor::to_bool(const std::any &value) {
  bool ret = false;
  if (auto val = std::any_cast<bool>(&value)) {
    ret = *val;
  } else if (auto val = std::any_cast<sjtu::int2048>(&value)) {
    ret = !(*val == sjtu::int2048(0));
  } else if (auto val = std::any_cast<double>(&value)) {
    ret = (*val != 0.0);
  } else if (auto val = std::any_cast<std::string>(&value)) {
    ret = !val->empty();
  }
  return std::any(ret);
}

std::any EvalVisitor::to_double(const std::any &value) {
  double ret = 0.0;
  if (auto val = std::any_cast<double>(&value)) {
    ret = *val;
  }
  if (auto val = std::any_cast<sjtu::int2048>(&value)) {
    ret = val->to_double();
  }
  if (auto val = std::any_cast<std::string>(&value)) {
    ret = std::stod(*val);
  }
  if (auto val = std::any_cast<bool>(&value)) {
    ret = *val ? 1.0 : 0.0;
  }
  return std::any(ret);
}

std::any EvalVisitor::to_string(const std::any &value) {
  std::string ret;
  if (auto val = std::any_cast<std::string>(&value)) {
    ret = *val;
  }
  if (auto val = std::any_cast<sjtu::int2048>(&value)) {
    ret = val->to_string();
  }
  if (auto val = std::any_cast<double>(&value)) {
    ret = std::to_string(*val);
  }
  if (auto val = std::any_cast<bool>(&value)) {
    ret = *val ? "True" : "False";
  }
  return std::any(ret);
}

std::any EvalVisitor::print(const std::vector<std::any> &args_) {
  std::vector<std::any> args;
  for (auto i : args_) {
    if (auto vec = std::any_cast<std::vector<std::any>>(&i)) {
      for (auto j : *vec) {
        args.push_back(getVariable(j));
      }
    } else {
      args.push_back(getVariable(i));
    }
  }
  for (size_t i = 0; i < args.size(); ++i) {
    if (i > 0) std::cout << " ";
    if (auto val = std::any_cast<std::string>(&args[i])) {
      std::cout << *val;
    } else if (auto val = std::any_cast<sjtu::int2048>(&args[i])) {
      std::cout << *val;
    } else if (auto val = std::any_cast<double>(&args[i])) {
      std::cout << *val;
    } else if (auto val = std::any_cast<bool>(&args[i])) {
      std::cout << std::setprecision(6) << (*val ? "True" : "False");
    } else {
      // std::cerr << "Unknown type in print function" << std::endl;
      std::cout << "None";
    }
  }
  std::cout << std::endl;
  return std::any();
}

std::any EvalVisitor::callSystemFunction(const std::string &name, const std::vector<std::any> &args) {
  if (name == "print") {
    // std::cerr << "Calling system function 'print', args size: " << args.size() << std::endl;
    return print(args);
  }
  if (name == "int") {
    if (args.size() != 1) {
      throw std::runtime_error("Too many arguments for int()");
    }
    return to_int(args[0]);
  }
  if (name == "float") {
    if (args.size() != 1) {
      throw std::runtime_error("Too many arguments for float()");
    }
    return to_double(args[0]);
  }
  if (name == "str") {
    if (args.size() != 1) {
      throw std::runtime_error("Too many arguments for str()");
    }
    return to_string(args[0]);
  }
  if (name == "bool") {
    if (args.size() != 1) {
      throw std::runtime_error("Too many arguments for bool()");
    }
    return to_bool(args[0]);
  }
  throw std::runtime_error("System function '" + name + "' not implemented");
}

std::any EvalVisitor::getVariable(std::any const &val) {
  if (auto namePtr = std::any_cast<std::string>(&val)) {
    std::string name = *namePtr;
    for (auto it = variables.rbegin(); it != variables.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }
  }
  return val;
}

void EvalVisitor::setVariable(const std::string &name, const std::any &value) {
  for (auto it = variables.rbegin(); it != variables.rend(); ++it) {
    if (it->find(name) != it->end()) {
      it->at(name) = value;
      return;
    }
  }
  // If variable not found in any scope, set it in the current scope
  variables.back()[name] = value;
}

std::any EvalVisitor::operate(const std::string &op, std::any left, std::any right) {
  if (op == "+") {
    if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
      auto leftStr = to_string(left);
      auto rightStr = to_string(right);
      return std::any(std::any_cast<std::string>(leftStr) + std::any_cast<std::string>(rightStr));
    }
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      throw std::runtime_error("TypeError: unsupported operand type(s) for +: 'str' and non-str");
    }
    if (left.type() == typeid(double) || right.type() == typeid(double)) {
      auto leftVal = to_double(left);
      auto rightVal = to_double(right);
      return std::any(std::any_cast<double>(leftVal) + std::any_cast<double>(rightVal));
    }
    auto leftVal = to_int(left);
    auto rightVal = to_int(right);
    return std::any(std::any_cast<sjtu::int2048>(leftVal) + std::any_cast<sjtu::int2048>(rightVal));
  }

  if (op == "-") {
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      throw std::runtime_error("TypeError: unsupported operand type(s) for -: 'str'");
    }
    if (left.type() == typeid(double) || right.type() == typeid(double)) {
      auto leftVal = to_double(left);
      auto rightVal = to_double(right);
      return std::any(std::any_cast<double>(leftVal) - std::any_cast<double>(rightVal));
    }
    auto leftVal = to_int(left);
    auto rightVal = to_int(right);
    return std::any(std::any_cast<sjtu::int2048>(leftVal) - std::any_cast<sjtu::int2048>(rightVal));
  }

  if (op == "*") {
    if (right.type() == typeid(std::string) && left.type() == typeid(sjtu::int2048)) {
      std::swap(left, right);
    }
    if (left.type() == typeid(std::string) && right.type() == typeid(sjtu::int2048)) {
      auto leftStr = to_string(left);
      auto rightInt = to_int(right);
      std::string result = "";
      if (std::any_cast<sjtu::int2048>(rightInt) <= sjtu::int2048(0)) {
        return std::any(result);
      } else {
        sjtu::int2048 times = std::any_cast<sjtu::int2048>(rightInt);
        for (sjtu::int2048 i = sjtu::int2048(0); i < times; i += sjtu::int2048(1)) {
          result += std::any_cast<std::string>(leftStr);
        }
        return std::any(result);
      }
    }
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      throw std::runtime_error("TypeError: unsupported operand type(s) for *: 'str' and non-int");
    }
    if (left.type() == typeid(double) || right.type() == typeid(double)) {
      auto leftVal = to_double(left);
      auto rightVal = to_double(right);
      return std::any(std::any_cast<double>(leftVal) * std::any_cast<double>(rightVal));
    }
    auto leftVal = to_int(left);
    auto rightVal = to_int(right);
    return std::any(std::any_cast<sjtu::int2048>(leftVal) * std::any_cast<sjtu::int2048>(rightVal));
  }

  if (op == "/") {
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      throw std::runtime_error("TypeError: unsupported operand type(s) for /: 'str'");
    }
    auto leftVal = to_double(left);
    auto rightVal = to_double(right);
    if (std::any_cast<double>(rightVal) == 0.0) {
      throw std::runtime_error("Division by zero");
    }
    return std::any(std::any_cast<double>(leftVal) / std::any_cast<double>(rightVal));
  }

  if (op == "//") {
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      throw std::runtime_error("TypeError: unsupported operand type(s) for //: 'str'");
    }
    if (left.type() == typeid(sjtu::int2048) && right.type() == typeid(sjtu::int2048)) {
      auto leftVal = to_int(left);
      auto rightVal = to_int(right);
      if (std::any_cast<sjtu::int2048>(rightVal) == sjtu::int2048(0)) {
        throw std::runtime_error("Division by zero");
      }
      return std::any(std::any_cast<sjtu::int2048>(leftVal) / std::any_cast<sjtu::int2048>(rightVal));
    }
    auto leftVal = to_double(left);
    auto rightVal = to_double(right);
    if (std::any_cast<double>(rightVal) == 0.0) {
      throw std::runtime_error("Division by zero");
    }
    return std::any(std::floor(std::any_cast<double>(leftVal) / std::any_cast<double>(rightVal)));
  }

  if (op == "%") {
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      throw std::runtime_error("TypeError: unsupported operand type(s) for %: 'str'");
    }
    if (left.type() == typeid(sjtu::int2048) && right.type() == typeid(sjtu::int2048)) {
      auto leftVal = to_int(left);
      auto rightVal = to_int(right);
      if (std::any_cast<sjtu::int2048>(rightVal) == sjtu::int2048(0)) {
        throw std::runtime_error("Modulo by zero");
      }
      return std::any(std::any_cast<sjtu::int2048>(leftVal) % std::any_cast<sjtu::int2048>(rightVal));
    }
    auto leftVal = to_double(left);
    auto rightVal = to_double(right);
    if (std::any_cast<double>(rightVal) == 0.0) {
      throw std::runtime_error("Modulo by zero");
    }
    return std::any(std::fmod(std::any_cast<double>(leftVal), std::any_cast<double>(rightVal)));
  }

  if (op == ">") {
    if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
      auto leftStr = to_string(left);
      auto rightStr = to_string(right);
      return std::any(std::any_cast<std::string>(leftStr) > std::any_cast<std::string>(rightStr));
    }
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      throw std::runtime_error("TypeError: '>' not supported between instances of 'str' and non-str");
    }
    if (left.type() == typeid(double) || right.type() == typeid(double)) {
      auto leftVal = to_double(left);
      auto rightVal = to_double(right);
      return std::any(std::any_cast<double>(leftVal) > std::any_cast<double>(rightVal));
    }
    auto leftVal = to_int(left);
    auto rightVal = to_int(right);
    return std::any(std::any_cast<sjtu::int2048>(leftVal) > std::any_cast<sjtu::int2048>(rightVal));
  }

  if (op == "<") {
    if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
      auto leftStr = to_string(left);
      auto rightStr = to_string(right);
      return std::any(std::any_cast<std::string>(leftStr) < std::any_cast<std::string>(rightStr));
    }
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      throw std::runtime_error("TypeError: '<' not supported between instances of 'str' and non-str");
    }
    if (left.type() == typeid(double) || right.type() == typeid(double)) {
      auto leftVal = to_double(left);
      auto rightVal = to_double(right);
      return std::any(std::any_cast<double>(leftVal) < std::any_cast<double>(rightVal));
    }
    auto leftVal = to_int(left);
    auto rightVal = to_int(right);
    return std::any(std::any_cast<sjtu::int2048>(leftVal) < std::any_cast<sjtu::int2048>(rightVal));
  }

  if (op == ">=") {
    auto result = std::any_cast<bool>(operate("<", right, left));
    return std::any(!result);
  }

  if (op == "<=") {
    auto result = std::any_cast<bool>(operate(">", right, left));
    return std::any(!result);
  }

  if (op == "==") {
    if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
      auto leftStr = to_string(left);
      auto rightStr = to_string(right);
      return std::any(std::any_cast<std::string>(leftStr) == std::any_cast<std::string>(rightStr));
    }
    if (left.type() == typeid(std::string) || right.type() == typeid(std::string)) {
      return std::any(false);
    }
    if (left.type() == typeid(double) || right.type() == typeid(double)) {
      auto leftVal = to_double(left);
      auto rightVal = to_double(right);
      return std::any(std::any_cast<double>(leftVal) == std::any_cast<double>(rightVal));
    }
    auto leftVal = to_int(left);
    auto rightVal = to_int(right);
    return std::any(std::any_cast<sjtu::int2048>(leftVal) == std::any_cast<sjtu::int2048>(rightVal));
  }

  if (op == "!=") {
    auto result = std::any_cast<bool>(operate("==", left, right));
    return std::any(!result);
  }
  
  return std::runtime_error("Invalid operator: " + op);
}

EvalVisitor::EvalVisitor() {
  variables.emplace_back();
}

std::any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
  auto statements = ctx->stmt();
  try {
    for (auto stmt : statements) {
      visit(stmt);
    }
  } catch (const std::runtime_error &e) {
    std::cerr << "Runtime Error: " << e.what() << std::endl;
    exit(1);
  }
  return std::any();
}

std::any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
  std::string funcName = ctx->NAME()->getText();
  auto paramsCtx = visit(ctx->parameters());
  auto typedArgsListCtx = std::any_cast<std::vector<Argument>>(paramsCtx);
  Function func;
  func.parameters = typedArgsListCtx;
  func.body = ctx->suite();
  functions[funcName] = func;
  return std::any();
}

std::any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
  if (ctx->typedargslist()) {
    return visit(ctx->typedargslist());
  }
  return std::vector<Argument>{};
}

std::any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx) {
  // all parameters' names are in tfpdef, while some may have default values
  auto parameters = ctx->tfpdef();
  auto parameters_with_defaults = ctx->test();
  std::vector<Argument> args;
  size_t total_params = parameters.size();
  size_t defaults_count = parameters_with_defaults.size();
  size_t non_defaults_count = total_params - defaults_count;
  for (size_t i = 0; i < total_params; ++i) {
    Argument arg;
    auto name = visit(parameters[i]);
    try {
      arg.name = std::any_cast<std::string>(name);
    } catch (const std::bad_any_cast &e) {  
      throw std::runtime_error("Parameter name is not a string");
    }
    if (i >= non_defaults_count) {
      auto default_value_ctx = parameters_with_defaults[i - non_defaults_count];
      arg.default_value = visit(default_value_ctx);
    } else {
      arg.default_value = std::any();
    }
    args.push_back(arg);
  }
  return args;
}

std::any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx) {
  return ctx->NAME()->getText();
}

std::any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
  if (ctx->simple_stmt()) {
    return visit(ctx->simple_stmt());
  } else if (ctx->compound_stmt()) {
    return visit(ctx->compound_stmt());
  }
  throw std::runtime_error("Invalid statement");
}

std::any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
  auto small_stmt = ctx->small_stmt();
  return visit(small_stmt);
}

std::any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
  if (ctx->expr_stmt()) {
    return visit(ctx->expr_stmt());
  } else if (ctx->flow_stmt()) {
    return visit(ctx->flow_stmt());
  }
  throw std::runtime_error("Invalid small statement");
}

std::any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
  auto testlist_ctx = ctx->testlist();
  auto value = std::any_cast<std::vector<std::any>>(visit(testlist_ctx.back()));
  // std::cerr << "Assigned value type: " << value.type().name() << std::endl;
  // auto testlistCtx = std::any_cast<std::vector<std::any>>(value);
  if (ctx->augassign()) {
    auto var = std::any_cast<std::vector<std::any>>(visit(testlist_ctx[0]))[0];
    auto varName = std::any_cast<std::string>(var);
    auto op = std::any_cast<std::string>(visit(ctx->augassign()));
    auto currentValue = getVariable(var); // left-hand side current value
    // operate after removing '=' from operator
    auto newValue = operate(op.substr(0, op.length() - 1), currentValue, value[0]);
    setVariable(varName, newValue);
  } else {
    // std::cerr << "Visiting normal assignment" << std::endl;
    for (int i = testlist_ctx.size() - 2; i >= 0; --i) {
      // std::cerr << "Assigning to variable(s) in testlist index " << i << std::endl;
      auto varList = std::any_cast<std::vector<std::any>>(visit(testlist_ctx[i]));
      for (int i = 0; i < varList.size(); ++i) {
        auto varName = std::any_cast<std::string>(varList[i]);
        setVariable(varName, value[i]);
      }
    }
  }
  return std::any();
}

std::any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx) {
  if (ctx->ADD_ASSIGN()) {
    return "+=";
  }
  if (ctx->SUB_ASSIGN()) {
    return "-=";
  }
  if (ctx->MULT_ASSIGN()) {
    return "*=";
  }
  if (ctx->DIV_ASSIGN()) {
    return "/=";
  }
  if (ctx->IDIV_ASSIGN()) {
    return "//=";
  }
  if (ctx->MOD_ASSIGN()) {
    return "%=";
  }
  throw std::runtime_error("Invalid augmented assignment operator");
}

std::any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
  std::pair<char, std::vector<std::any>> ret;
  if (ctx->break_stmt()) {
    return visit(ctx->break_stmt());
  }
  if (ctx->continue_stmt()) {
    return visit(ctx->continue_stmt());
  }
  if (ctx->return_stmt()) {
    return visit(ctx->return_stmt());
  }
  throw std::runtime_error("Invalid flow statement");
}

std::any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) {
  return std::pair<char, std::vector<std::any>>('b', {});
}

std::any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) {
  return std::pair<char, std::vector<std::any>>('c', {});
}

std::any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) {
  std::vector<std::any> returnValues;
  if (ctx->testlist()) {
    auto tests = ctx->testlist()->test();
    for (auto test : tests) {
      auto value = visit(test);
      returnValues.push_back(value);
    }
  }
  return std::pair<char, std::vector<std::any>>('r', returnValues);
}

std::any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
  if (ctx->if_stmt()) {
    return visit(ctx->if_stmt());
  }
  if (ctx->while_stmt()) {
    return visit(ctx->while_stmt());
  }
  if (ctx->funcdef()) {
    return visit(ctx->funcdef());
  }
  throw std::runtime_error("Invalid compound statement");
}

std::any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
  auto tests = ctx->test();
  auto suites = ctx->suite();
  size_t conditions_count = tests.size();
  for (size_t i = 0; i < conditions_count; ++i) {
    auto conditionValue = visit(tests[i]);
    if (std::any_cast<bool>(to_bool(conditionValue))) {
      return visit(suites[i]);
    }
  }
  // has an else statement
  if (suites.size() > conditions_count) {
    return visit(suites.back());
  }
  return std::any();
}

std::any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
  auto conditionCtx = ctx->test();
  auto bodyCtx = ctx->suite();
  while (true) {
    auto conditionValue = visit(conditionCtx);
    if (!std::any_cast<bool>(to_bool(conditionValue))) {
      break;
    }
    auto result = visit(bodyCtx);
    if (result.type() == typeid(std::pair<char, std::vector<std::any>>)) {
      auto flowControl = std::any_cast<std::pair<char, std::vector<std::any>>>(result);
      if (flowControl.first == 'b') { // break
        break;
      } else if (flowControl.first == 'c') { // continue
        continue;
      } else if (flowControl.first == 'r') { // return
        return result;
      }
    }
  }
  return std::any();
}

std::any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
  if (ctx->simple_stmt()) {
    return visit(ctx->simple_stmt());
  }
  auto statements = ctx->stmt();
  for (auto stmt : statements) {
    auto result = visit(stmt);
    if (result.type() == typeid(std::pair<char, std::vector<std::any>>)) {
      return result;
    }
  }
  return std::any();
}

std::any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
  // std::cerr << "Visiting test" << std::endl;
  return visit(ctx->or_test());
}

std::any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
  auto andTests = ctx->and_test();
  size_t tests_count = andTests.size();
  std::any result = visit(andTests[0]);
  for (size_t i = 1; i < tests_count; ++i) {
    // return true if any is true
    if (std::any_cast<bool>(to_bool(result))) {
      return result;
    }
    auto nextValue = visit(andTests[i]);
    result = nextValue;
  }
  return result;
}

std::any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
  auto notTests = ctx->not_test();
  size_t tests_count = notTests.size();
  std::any result = visit(notTests[0]);
  for (size_t i = 1; i < tests_count; ++i) {
    // return false if any is false
    if (!std::any_cast<bool>(to_bool(result))) {
      return result;
    }
    auto nextValue = visit(notTests[i]);
    result = nextValue;
  }
  return result;
}

std::any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
  if (ctx->NOT()) {
    auto value = visit(ctx->not_test());
    bool boolValue = std::any_cast<bool>(to_bool(value));
    return std::any(!boolValue);
  } else {
    return visit(ctx->comparison());
  }
}

std::any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
  auto arithExprs = ctx->arith_expr();
  auto compOps = ctx->comp_op();
  size_t exprs_count = arithExprs.size();
  if (exprs_count == 1) {
    return visit(arithExprs[0]);
  }
  std::any leftValue = visit(arithExprs[0]);
  for (size_t i = 0; i < compOps.size(); ++i) {
    auto rightValue = visit(arithExprs[i + 1]);
    auto op = std::any_cast<std::string>(visit(compOps[i]));
    auto comparisonResult = operate(op, leftValue, rightValue);
    if (!std::any_cast<bool>(comparisonResult)) {
      return std::any(false);
    }
    leftValue = rightValue;
  }
  return std::any(true);
}

std::any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx) {
  if (ctx->LESS_THAN()) {
    return "<";
  }
  if (ctx->GREATER_THAN()) {
    return ">";
  }
  if (ctx->EQUALS()) {
    return "==";
  }
  if (ctx->GT_EQ()) {
    return ">=";
  }
  if (ctx->LT_EQ()) {
    return "<=";
  }
  if (ctx->NOT_EQ_2()) {
    return "!=";
  }
  throw std::runtime_error("Invalid comparison operator");
}

std::any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
  auto terms = ctx->term();
  size_t terms_count = terms.size();
  std::any result = visit(terms[0]);
  if (terms_count == 1) {
    return result;
  }
  auto addorsubOps = ctx->addorsub_op();
  for (size_t i = 0; i < addorsubOps.size(); ++i) {
    auto nextValue = visit(terms[i + 1]);
    auto op = std::any_cast<std::string>(visit(addorsubOps[i]));
    result = operate(op, result, nextValue);
  }
  return result;
}

std::any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) {
  if (ctx->ADD()) {
    return "+";
  }
  if (ctx->MINUS()) {
    return "-";
  }
  throw std::runtime_error("Invalid add or sub operator");
}

std::any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
  auto factors = ctx->factor();
  size_t factors_count = factors.size();
  auto result = visit(factors[0]);
  if (factors_count == 1) {
    // std::cerr << "Term with single factor" << std::endl;
    // std::cerr << std::any_cast<std::string>(to_string(result)) << std::endl;
    return result;
  }
  auto muldivmodOps = ctx->muldivmod_op();
  for (size_t i = 0; i < muldivmodOps.size(); ++i) {
    auto nextValue = visit(factors[i + 1]);
    auto op = std::any_cast<std::string>(visit(muldivmodOps[i]));
    result = operate(op, result, nextValue);
  }
  return result;
}

std::any EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) {
  if (ctx->STAR()) {
    return "*";
  }
  if (ctx->DIV()) {
    return "/";
  }
  if (ctx->IDIV()) {
    return "//";
  }
  if (ctx->MOD()) {
    return "%";
  }
  throw std::runtime_error("Invalid mul/div/mod operator");
}

std::any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
  if (ctx->factor()) {
    auto value = visit(ctx->factor());
    if (ctx->ADD()) {
      if (value.type() == typeid(double)) {
        return value;
      }
      if (value.type() == typeid(sjtu::int2048)) {
        return value;
      }
      if (value.type() == typeid(bool)) {
        bool boolValue = std::any_cast<bool>(value);
        return std::any(boolValue ? sjtu::int2048(1) : sjtu::int2048(0));
      }
      throw std::runtime_error("TypeError: bad operand type for unary -");
    }
    if (ctx->MINUS()) {
      if (value.type() == typeid(double)) {
        return std::any(-std::any_cast<double>(value));
      }
      if (value.type() == typeid(sjtu::int2048)) {
        return std::any(-std::any_cast<sjtu::int2048>(value));
      }
      if (value.type() == typeid(bool)) {
        bool boolValue = std::any_cast<bool>(value);
        return std::any(boolValue ? sjtu::int2048(-1) : sjtu::int2048(0));
      }
      throw std::runtime_error("TypeError: bad operand type for unary -");
    }
  } else if (ctx->atom_expr()) {
    // auto tmp = visit(ctx->atom_expr());
    // if (auto vec = std::any_cast<std::vector<std::any>>(&tmp)) {
    //   std::cerr << "Visiting factor returning vector of size: " << vec->size() << std::endl;
    // }
    return visit(ctx->atom_expr());
  }
  throw std::runtime_error("Invalid factor");
}

std::any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
  // std::cerr << "Visiting atom_expr" << std::endl;
  auto atomCtx = visit(ctx->atom());
  if (atomCtx.type() == typeid(std::string) && ctx->trailer()) {
    // function call
    std::string funcName = std::any_cast<std::string>(atomCtx);
    if (systemFunctions.count(funcName)) {
      // handle system functions
      auto trailerCtx = visit(ctx->trailer());
      std::vector<std::any> argValues;
      for (auto &argCtx : std::any_cast<std::vector<Argument>>(trailerCtx)) {
        argValues.push_back(argCtx.default_value);
      }
      return callSystemFunction(funcName, argValues);
    }
    auto funcIt = functions.find(funcName);
    if (funcIt == functions.end()) {
      throw std::runtime_error("Function '" + funcName + "' not defined");
    }
    Function func = funcIt->second;
    // evaluate arguments
    std::vector<std::any> argValues;
    auto trailerCtx = visit(ctx->trailer());
    for (auto &argCtx : std::any_cast<std::vector<Argument>>(trailerCtx)) {
      argValues.push_back(argCtx.default_value);
    }
    // create new variable scope
    variables.emplace_back();
    // set parameters
    size_t paramCount = func.parameters.size();
    size_t argCount = argValues.size();
    for (size_t i = 0; i < paramCount; ++i) {
      Argument param = func.parameters[i];
      if (i < argCount) {
        setVariable(param.name, argValues[i]);
      } else {
        if (param.default_value.has_value()) {
          setVariable(param.name, param.default_value);
        } else {
          throw std::runtime_error("TypeError: missing required positional argument '" + param.name + "'");
        }
      }
    }
    // execute function body
    auto result = visit(func.body);
    // pop variable scope
    variables.pop_back();
    if (result.type() == typeid(std::pair<char, std::vector<std::any>>)) {
      auto flowControl = std::any_cast<std::pair<char, std::vector<std::any>>>(result);
      if (flowControl.first == 'r') { // return
        if (!flowControl.second.empty()) {
          return flowControl.second[0];
        } else {
          return std::any();
        }
      }
    }
    return std::any();
  }
  return atomCtx;
}

std::any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
  if (ctx->arglist()) {
    auto argsCtx = visit(ctx->arglist());
    return argsCtx;
  }
  return std::vector<Argument>();
}

std::any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
  if (ctx->NAME()) {
    return ctx->NAME()->getText();
  }
  if (ctx->NUMBER()) {
    std::string numText = ctx->NUMBER()->getText();
    if (numText.find('.') != std::string::npos || numText.find('e') != std::string::npos || numText.find('E') != std::string::npos) {
      // float
      double value = std::stod(numText);
      return std::any(value);
    } else {
      // int
      sjtu::int2048 value;
      value.read(numText);
      return std::any(value);
    }
  }
  if (ctx->STRING(0)) {
    std::vector<std::any> ret;
    auto strs = ctx->STRING();
    for (auto strCtx : strs) {
      std::string rawStr = strCtx->getText();
      // remove the surrounding quotes
      char quote = rawStr[0];
      std::string content = rawStr.substr(1, rawStr.length() - 2);
      // handle escape sequences
      std::string processedStr;
      for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == '\\' && i + 1 < content.length()) {
          char nextChar = content[i + 1];
          if (nextChar == 'n') {
            processedStr += '\n';
            i++;
          } else if (nextChar == 't') {
            processedStr += '\t';
            i++;
          } else if (nextChar == 'r') {
            processedStr += '\r';
            i++;
          } else if (nextChar == '\\') {
            processedStr += '\\';
            i++;
          } else if (nextChar == quote) {
            processedStr += quote;
            i++;
          } else {
            processedStr += content[i];
          }
        } else {
          processedStr += content[i];
        }
      }
      // std::cerr << "Processed string: " << processedStr << std::endl;
      ret.push_back(processedStr);
    }
    return std::any(ret);
  }
  if (ctx->TRUE()) {
    return std::any(true);
  }
  if (ctx->FALSE()) {
    return std::any(false);
  }
  if (ctx->NONE()) {
    return std::any();
  }
  if (ctx->test()) {
    return visit(ctx->test());
  }
  if (ctx->format_string()) {
    return visit(ctx->format_string());
  }
  throw std::runtime_error("Invalid atom");
}

std::any EvalVisitor::visitFormat_string(Python3Parser::Format_stringContext *ctx) {
  auto strings = ctx->FORMAT_STRING_LITERAL();
  auto tests = ctx->testlist();
  std::string result;
  int i = 0, j = 0;
  while (i < strings.size() || j < tests.size()) {
    if (i < strings.size() && (j >= tests.size() || strings[i]->getSymbol()->getStartIndex() < tests[j]->getStart()->getStartIndex())) {
      auto str = strings[i]->getText();
      auto pos = str.find("{{");
      while (pos != std::string::npos) {
        str.replace(pos, 2, "{");
        pos = str.find("{{", pos + 1);
      }
      pos = str.find("}}");
      while (pos != std::string::npos) {
        str.replace(pos, 2, "}");
        pos = str.find("}}", pos + 1);
      }
      result += str;
      i++;
    } else if (j < tests.size()) {
      auto value = visit(tests[j]);
      if (auto val = std::any_cast<std::vector<std::any>>(&value)) {
        for (auto element : *val) {
          auto strVal = to_string(element);
          result += std::any_cast<std::string>(strVal);
        }
      }
      j++;
    }
  }
  return std::any(result);
}

std::any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
  // std::cerr << "Visiting testlist" << std::endl;
  std::vector<std::any> values;
  auto tests = ctx->test();
  for (auto test : tests) {
    auto value = visit(test);
    if (auto val = std::any_cast<std::vector<std::any>>(&value)) {
      for (auto element : *val) {
        values.push_back(element);
      }
    } else {
      values.push_back(value);
    }
  }
  return values;
}

std::any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
  std::vector<Argument> arglist;
  auto args = ctx->argument();
  for (auto argCtx : args) {
    auto tmp = visit(argCtx);
    arglist.push_back(std::any_cast<Argument>(tmp));
  }
  return arglist;
}

std::any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
  Argument arg;
  auto tests = ctx->test();
  if (tests.size() == 1) {
    arg.default_value = visit(tests[0]);
  } else {
    // std::cerr << "Visiting argument with name and default value" << std::endl;
    arg.name = std::any_cast<std::string>(visit(tests[0]));
    arg.default_value = visit(tests[1]);
  }
  return arg;
}