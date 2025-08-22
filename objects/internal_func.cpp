#include "internal_func.hpp"
#include "../error.hpp"
#include "callable.hpp"

namespace internal_func {
  std::vector<Object*> InternalFunction::on_call(std::vector<Object*> args) {
    try {
      return func(args);
    } catch (error::eval_error& e) {
      if (e.location.end.filename == nullptr)
        e.location = this->loc;
      throw e;
    } 
  }
  InternalFunction::InternalFunction(std::function<std::vector<Object*>(std::vector<Object*>)> func, std::vector<Type*> arg_types,std::string& file,int line) : 
    Callable(parser::location(&file,line),std::move(arg_types)) {
    this->func = func;
  }
  bool InternalFunction::operator==(const Object* that) const {
    if (typeid(*that) != typeid(*this)) return false;
    auto _that = static_cast<const InternalFunction*>(that);
    if (this->func.target_type() != _that->func.target_type()) return false;
    return this->func.target<std::function<std::vector<Object*>(std::vector<Object*>)>>() == 
          _that->func.target<std::function<std::vector<Object*>(std::vector<Object*>)>>();
  }
}
