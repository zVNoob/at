#include "error.hpp"
#include "array.hpp"
#include "location.hh"

#include "object.hpp"

#include "integer.hpp"
#include "string.hpp"
#include "fraction.hpp"

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
  if (dynamic_cast<fraction::Fraction*>(obj)) {
    output << static_cast<fraction::Fraction*>(obj)->value;
  } else
  if (dynamic_cast<array::Array*>(obj)) {
    output << '[';
    for (size_t i = 0; i < static_cast<array::Array*>(obj)->elements.size(); i++) {
      if (i > 0) output << ", ";
      dump_object(static_cast<array::Array*>(obj)->elements[i].get());
    }
    output << ']';
  } else
  output << "<<unknown>>";
}

void StreamErrorReporter::on_orphan_value(std::shared_ptr<object::Object> value) {
  if (value) {
    dump_object(value.get());
    output << std::endl;
  }
}

}
