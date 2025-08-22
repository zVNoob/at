#include "internal_func.hpp"
#include "../error.hpp"
#include "callable.hpp"

namespace internal_func {
using namespace callable;
using namespace type;
  arg_list InternalFunction::on_call(arg_list args) {
    try {
      return func(args);
    } catch (error::eval_error& e) {
      if (e.location.end.filename == nullptr)
        e.location = this->loc;
      throw e;
    } 
  }
  InternalFunction::InternalFunction(std::function<arg_list(arg_list)> func, std::source_location loc) : 
    func(std::move(func)),
    Callable(parser::location(0,loc.line(),loc.column())) {
      this->source_name = loc.file_name();
      this->loc = parser::location(&this->source_name,loc.line(),loc.column());
    }
  bool InternalFunction::operator==(const Object* that) const {
    if (typeid(*that) != typeid(*this)) return false;
    auto _that = static_cast<const InternalFunction*>(that);
    if (this->func.target_type() != _that->func.target_type()) return false;
    return this->func.target<std::function<arg_list(arg_list)>>() == 
          _that->func.target<std::function<arg_list(arg_list)>>();
  }
}
