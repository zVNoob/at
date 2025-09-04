#include "error.hpp"
#include "location.hh"

#include "object.hpp"

#include "integer.hpp"
#include "string.hpp"

namespace error {

void StreamErrorReporter::report(lexer::Lexer* lexer,parser::location loc,const std::string& msg) {
  output << loc << ": " << msg << std::endl;
}

void StreamErrorReporter::dump_object(object::Object* obj) {
  if (dynamic_cast<integer::Integer*>(obj)) {
    output << static_cast<integer::Integer*>(obj)->value;
  } else
    if (dynamic_cast<string::String*>(obj)) {
      output << static_cast<string::String*>(obj)->value;
    } else
  output << "<<unknown>>" << std::endl;
}

void StreamErrorReporter::orphan_value(std::shared_ptr<object::Object> value) {
  dump_object(value.get());
  output << std::endl;
}

}
