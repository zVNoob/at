#include "error.hpp"
#include "location.hh"
#include <numeric>

void error::StreamErrorReporter::report(lexer::Lexer* lexer,parser::location loc,const std::string& msg) {
  output << loc << ": " << msg << std::endl;
}

void error::StreamErrorReporter::orphan_return(const std::vector<std::shared_ptr<object::Object>>& obj) {
  output << std::accumulate(std::next(obj.begin()), 
                            obj.end(), 
                            obj[0]->to_string(), 
                            [](std::string a, std::shared_ptr<object::Object> b) { 
                                return a + ", " + b->to_string(); 
                              }
                            ) << std::endl;
}
