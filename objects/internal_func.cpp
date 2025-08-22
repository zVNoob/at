#include "internal_func.hpp"
#include "../error.hpp"
#include "callable.hpp"

namespace internal_func {
  arg_list InternalFunction::on_call(arg_list args) {
    try {
      return func(args);
    } catch (error::eval_error& e) {
      if (e.location.end.filename == nullptr)
        e.location = this->loc;
      throw e;
    } 
  }
  InternalFunction::InternalFunction(std::function<arg_list(arg_list)> func, std::string& file,int line) : 
    func(std::move(func)),
    Callable(parser::location(&file,line)) {
  }
  bool InternalFunction::operator==(const Object* that) const {
    if (typeid(*that) != typeid(*this)) return false;
    auto _that = static_cast<const InternalFunction*>(that);
    if (this->func.target_type() != _that->func.target_type()) return false;
    return this->func.target<std::function<arg_list(arg_list)>>() == 
          _that->func.target<std::function<arg_list(arg_list)>>();
  }
}
