#include "parser_helper.hpp"

#include "callable.hpp"
#include "error.hpp"
#include "object.hpp"
#include "type.hpp" // IWYU pragma: keep
#include "tuple.hpp"
#include "array.hpp"
#include "variable.hpp"
#include <memory>

namespace parser {
std::shared_ptr<object::Object> exec_binary_op(std::shared_ptr<object::Object> lhs, 
                         std::shared_ptr<object::Object> rhs, 
                         const std::string& op,parser::location loc) {
  if (lhs->type == nullptr) throw error::eval_error("No type found",loc);
  object::Object* obj = lhs->type->members[op].get();
  if (dynamic_cast<callable::Callable*>(obj) == nullptr) 
    throw error::eval_error("This operator is not defined for that data type",loc);
  return static_cast<callable::Callable*>(obj)->on_call({lhs,rhs},loc);
}

std::shared_ptr<object::Object> exec_unary_op(std::shared_ptr<object::Object> rhs, 
                                              const std::string& op,parser::location loc) {
  if (rhs->type == nullptr) throw error::eval_error("No type found",loc);
  object::Object* obj = rhs->type->members[op].get();
  if (dynamic_cast<callable::Callable*>(obj) == nullptr) 
    throw error::eval_error("This operator is not defined for that data type",loc);
  return static_cast<callable::Callable*>(obj)->on_call({rhs},loc);
}

std::shared_ptr<object::Object> exec_call(std::shared_ptr<object::Object> obj, 
                                         std::vector<std::shared_ptr<object::Object>> args, 
                                         std::string method,parser::location loc) {
  if (obj->type == nullptr) throw error::eval_error("No type found",loc);
  object::Object* obj2 = obj->type->members[method].get();
  if (dynamic_cast<callable::Callable*>(obj2) == nullptr) 
    throw error::eval_error("This method is not defined for that data type",loc);
  std::vector<std::shared_ptr<object::Object>> args2 = {obj};
  args2.insert(args2.end(),args.begin(),args.end());
  return static_cast<callable::Callable*>(obj2)->on_call(args2,loc);
}

std::shared_ptr<object::Object> exec_assign_single(std::shared_ptr<object::Object> lhs,
                        std::shared_ptr<object::Object> rhs,
                        parser::location loc) {
    if (lhs->type == nullptr) throw error::eval_error("No type found",loc);
    object::Object* obj = lhs->type->members["="].get();
    if (dynamic_cast<callable::Callable*>(obj) == nullptr) 
      throw error::eval_error("This data type is not assignable",loc);
    return static_cast<callable::Callable*>(obj)->on_call({lhs,rhs},loc);
}
std::shared_ptr<object::Object> exec_assign(std::vector<std::shared_ptr<object::Object>> vars, 
                 std::shared_ptr<object::Object> rhs, 
                 parser::location loc) {
  if (vars.size() == 1) 
    return exec_assign_single(vars[0],rhs,loc);
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
    results[i] = exec_assign_single(vars[i],std::static_pointer_cast<tuple::Tuple>(rhs)->values[i],loc);
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
    std::vector<std::shared_ptr<object::Object>> results(vars.size());
    for (int i = 0; i < vars.size(); i++) {
      lexer->current_scope->add_member(std::static_pointer_cast<variable::Variable>(vars[i])->name,
                                       std::static_pointer_cast<tuple::Tuple>(rhs)->values[i]);
      results[i] = std::static_pointer_cast<tuple::Tuple>(rhs)->values[i];
    }
  }
  return std::make_shared<tuple::Tuple>(vars);
}
std::shared_ptr<object::Object> exec_build_array(std::vector<std::shared_ptr<object::Object>> elements,parser::location loc) {
  if (elements[0]->type == nullptr) throw error::eval_error("No type found",loc);
  auto result = std::make_shared<array::Array>(elements[0]->type);
  for (auto& element : elements) {
    if (element->type != elements[0]->type)
      throw error::eval_error("Array elements must be of the same type",loc);
    result->elements.push_back(element);
  }
  return result;
}


}
