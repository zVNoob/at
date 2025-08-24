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
    func(std::move(func)), source_name(loc.file_name()),
    Callable(parser::location(0,loc.line(),loc.column())) {
      sloc = loc;
      this->loc = parser::location(&this->source_name,loc.line(),loc.column());
  }

  std::string InternalFunction::to_string() const {
    return std::string(sloc.function_name())
            + "(" + 
              sloc.file_name() + ":" + 
              std::to_string(sloc.line()) + ":" + std::to_string(sloc.column()) + 
            ")";
  }

}
