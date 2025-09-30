#include "parser_helper.hpp"

#include "ast.hpp"
#include "call.hpp"
#include "lexer.hpp"
#include "loop.hpp"
#include "call.hpp"
#include "callable.hpp"
#include "error.hpp"
#include "integer.hpp"
#include "object.hpp"
#include "type.hpp" // IWYU pragma: keep
#include "tuple.hpp"
#include "array.hpp"
#include "unknown_variable.hpp"
#include "variable.hpp"
#include <memory>

namespace parser {
std::shared_ptr<object::Object> exec_binary_op(std::shared_ptr<object::Object> lhs, 
                         std::shared_ptr<object::Object> rhs, 
                         const std::string& op,parser::location loc) {
  if (lhs->type == nullptr) throw error::eval_error("No type found",loc);
  auto obj = lhs->type->members[op];
  if (dynamic_cast<typed_func::TypedFunction*>(obj.get()) == nullptr) 
    throw error::eval_error("This operator is not defined for that data type",loc);
  if (dynamic_cast<ast::Ast*>(lhs.get()) || dynamic_cast<ast::Ast*>(rhs.get()))
    return std::make_shared<ast::Call>(ast::Call(std::static_pointer_cast<typed_func::TypedFunction>(obj),{lhs,rhs},loc));
  return static_cast<callable::Callable*>(obj.get())->on_call({lhs,rhs},loc);
}

std::shared_ptr<object::Object> exec_unary_op(std::shared_ptr<object::Object> rhs, 
                                              const std::string& op,parser::location loc) {
  if (rhs->type == nullptr) throw error::eval_error("No type found",loc);
  auto obj = rhs->type->members[op];
  if (dynamic_cast<typed_func::TypedFunction*>(obj.get()) == nullptr) 
    throw error::eval_error("This operator is not defined for that data type",loc);
  if (dynamic_cast<ast::Ast*>(rhs.get()))
    return std::make_shared<ast::Call>(ast::Call(std::static_pointer_cast<typed_func::TypedFunction>(obj),{rhs},loc));
  return static_cast<callable::Callable*>(obj.get())->on_call({rhs},loc);
}

std::shared_ptr<object::Object> exec_call(std::shared_ptr<object::Object> obj, 
                                         std::vector<std::shared_ptr<object::Object>> args, 
                                         std::string method,parser::location loc) {
  if (obj->type == nullptr) throw error::eval_error("No type found",loc);
  auto obj2 = obj->type->members[method];
  if (dynamic_cast<typed_func::TypedFunction*>(obj2.get()) == nullptr) 
    throw error::eval_error("This method is not defined for that data type",loc);
  std::vector<std::shared_ptr<object::Object>> args2 = {obj};
  args2.insert(args2.end(),args.begin(),args.end());
  bool has_ast = false;
  for (auto& arg : args2) {
    if (dynamic_cast<ast::Ast*>(arg.get())) {
      has_ast = true;
      break;
    }
  }
  if (has_ast)
    return std::make_shared<ast::Call>(ast::Call(std::static_pointer_cast<typed_func::TypedFunction>(obj2),args2,loc));
  return static_cast<callable::Callable*>(obj2.get())->on_call(args2,loc);
}

std::shared_ptr<object::Object> exec_assign_single(std::shared_ptr<object::Object> lhs,
                        std::shared_ptr<object::Object> rhs,
                        lexer::Lexer* lexer,
                        parser::location loc) {
    if (lhs->type == nullptr) throw error::eval_error("No type found",loc);
    auto op = lhs->type->members["="];
    if (dynamic_cast<callable::Callable*>(op.get()) == nullptr) 
      throw error::eval_error("This data type is not assignable",loc);
    if (dynamic_cast<ast::Ast*>(lhs.get()) || dynamic_cast<ast::Ast*>(rhs.get())) {
      std::shared_ptr<ast::UnknownVariable> obj;
      if (dynamic_cast<ast::UnknownVariable*>(lhs.get()))
        obj = std::static_pointer_cast<ast::UnknownVariable>(lhs);
      else
        obj = make_shared<ast::UnknownVariable>(std::static_pointer_cast<variable::Variable>(lhs));
      lexer->current_scope->add_statement(std::make_shared<ast::Assign_Operator>(std::static_pointer_cast<typed_func::TypedFunction>(op), obj, rhs, loc));
      return nullptr;
    }
    return static_cast<callable::Callable*>(op.get())->on_call({lhs,rhs},loc);
}
std::shared_ptr<object::Object> exec_assign(std::vector<std::shared_ptr<object::Object>> vars, 
                 std::shared_ptr<object::Object> rhs, 
                 lexer::Lexer* lexer,
                 parser::location loc) {
  if (vars.size() == 1) 
    return exec_assign_single(vars[0],rhs, lexer,loc);
  if (dynamic_cast<tuple::Tuple*>(rhs.get()) == nullptr) 
    throw error::eval_error("Cannot unpack non-tuple",loc);
  if (vars.size() > std::static_pointer_cast<tuple::Tuple>(rhs)->values.size())
    throw error::eval_error("Tuple assignment size mismatch",loc);
  for (auto& var : vars) {
    if (std::static_pointer_cast<variable::Variable>(var)->value == nullptr)
      throw error::eval_error("Cannot assign to undeclared variable",loc);
    if (std::static_pointer_cast<variable::Variable>(var)->readonly)
      throw error::eval_error("Cannot assign to readonly variable",loc);
  }
  std::vector<std::shared_ptr<object::Object>> results(vars.size());
  for (int i = 0; i < vars.size(); i++) {
    results[i] = exec_assign_single(vars[i],std::static_pointer_cast<tuple::Tuple>(rhs)->values[i], lexer,loc);
  }
  return std::make_shared<tuple::Tuple>(results);
}

std::shared_ptr<object::Object> exec_declare(std::vector<std::shared_ptr<object::Object>> vars,
                  std::shared_ptr<object::Object> rhs, lexer::Lexer* lexer,parser::location loc) {
  if (vars.size() == 1) 
    lexer->current_scope->add_member(std::static_pointer_cast<variable::Variable>(vars[0])->name, rhs);
  else {
    if (dynamic_cast<tuple::Tuple*>(rhs.get()) == nullptr) 
      throw error::eval_error("Cannot unpack non-tuple",loc);
    if (vars.size() > std::static_pointer_cast<tuple::Tuple>(rhs)->values.size())
      throw error::eval_error("Tuple assignment size mismatch",loc);
    for (auto& var : vars) {
      if (!std::static_pointer_cast<variable::Variable>(var)->re_declarable)
        throw error::eval_error("Cannot re-declare variable",loc);
    }
    for (int i = 0; i < vars.size(); i++) {
      lexer->current_scope->add_member(std::static_pointer_cast<variable::Variable>(vars[i])->name,
                                       std::static_pointer_cast<tuple::Tuple>(rhs)->values[i]);
    }
  }
  return nullptr;
}
std::shared_ptr<object::Object> exec_build_array(std::vector<std::shared_ptr<object::Object>> elements,parser::location loc) {
  if (elements.size() == 0) throw error::eval_error("Cannot deduce array type",loc);
  if (elements[0]->type == nullptr) throw error::eval_error("No type found",loc);
  auto result = std::make_shared<array::Array>();
  result->type = array::get_Array_type(elements[0]->type);
  for (auto& element : elements) {
    if (element->type != elements[0]->type)
      throw error::eval_error("Array elements must be of the same type",loc);
    result->elements.push_back(element);
  }
  return result;
}

void on_orphan_value(std::shared_ptr<object::Object> value, lexer::Lexer* lexer, error::ErrorReporter* err_rp) {
  if (dynamic_cast<ast::Ast*>(value.get()) != nullptr) {
    if (lexer->current_scope->parent == nullptr)
      err_rp->orphan_value(dynamic_pointer_cast<ast::Ast>(value)->eval());
    else
      lexer->current_scope->add_statement(std::static_pointer_cast<ast::Ast>(value));
  } else if (lexer->current_scope->parent == nullptr && lexer->current_scope->auto_resolve) {
    err_rp->orphan_value(value);
  }
}

std::shared_ptr<object::Object> exec_constructor(std::shared_ptr<object::Object> type,
                                                 std::vector<std::shared_ptr<object::Object>> args,
                                                 parser::location loc) {
  auto temp = std::static_pointer_cast<type::Type>(type);
  auto func_obj = temp->members[""];
  if (func_obj == nullptr) throw error::eval_error("This type has no constructor",loc);
  if (dynamic_cast<callable::Callable*>(func_obj.get()) == nullptr) 
    throw error::eval_error("This type has no constructor",loc);
  auto arg = args;
  arg.insert(arg.begin(),type);
    bool has_ast = false;
  for (auto& a : arg) {
    if (dynamic_cast<ast::Ast*>(a.get())) {
      has_ast = true;
      break;
    }
  }
  if (has_ast)
    return std::make_shared<ast::Call>(ast::Call(std::static_pointer_cast<typed_func::TypedFunction>(func_obj),arg,loc));
  return static_cast<callable::Callable*>(func_obj.get())->on_call(arg,loc);
}

std::shared_ptr<object::Object> exec_conditional(std::shared_ptr<object::Object> cond, std::shared_ptr<object::Object> lhs, std::shared_ptr<object::Object> rhs,parser::location loc) {
  if (cond->type != integer::Get_Integer_type())
    throw error::eval_error("Condition must be an integer",loc);
  if (lhs->type != rhs->type)
    throw error::eval_error("Branches of conditional must be of the same type",loc);
  if (std::static_pointer_cast<integer::Integer>(cond)->value == BigInt(0))
    return rhs;
  else
    return lhs;
}

void push_scope_block(lexer::Lexer *lexer) {
  auto new_scope = std::make_shared<scope::Scope>();
  new_scope->parent = lexer->current_scope;
  lexer->current_scope = new_scope;
}

std::shared_ptr<object::Object> pop_scope_block(lexer::Lexer *lexer) {
  auto current_scope = lexer->current_scope;
  lexer->current_scope = lexer->current_scope->parent;
  return current_scope;
}

void push_scope_loop(lexer::Lexer *lexer, parser::location loc, std::shared_ptr<object::Object> cond) {
  auto new_scope = std::make_shared<ast::Loop>(cond, loc);
  new_scope->parent = lexer->current_scope;
  lexer->current_scope = new_scope;
}

std::shared_ptr<object::Object> pop_scope_loop(lexer::Lexer *lexer) {
  auto current_scope = lexer->current_scope;
  lexer->current_scope = lexer->current_scope->parent;
  return current_scope;
}

}
