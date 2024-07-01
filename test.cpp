#include "location.hh"
#include "misc/stringlist.hpp"
#include <cstdlib>
#include <iostream>
#include <variant>

void lex(yy::location &loc, StringList &list) {
  while (1) {
    if (loc.end.line > list.size()) {
      std::cout << "EOF" << std::endl;
      exit(0);
    }
    std::string spacing;
    std::string result;
    for (; loc.end.column < list[loc.end.line - 1].size(); ++loc.end.column) {
      char token = list[loc.end.line - 1][loc.end.column - 1];
      if (token == ' ' || token == '\t' || token == '\r') {
        spacing += token;
        continue;
      }
      if (token >= '0' && token <= '9') {
        if (result.empty())
          loc.step();
        else if (!spacing.empty()) {
          result += spacing;
          spacing.clear();
        }
        result += token;
        continue;
      }
      if (!result.empty()) {
        std::cout << result << std::endl;
        result.clear();
        spacing.clear();
        return;
      } else {
        loc.step();
        loc.end.column++;
        std::cout << token << std::endl;

        return;
      }
    }
    loc.end.line++;
    loc.end.column = 1;
  }
}

int main() {
  yy::location loc;
  StringList list;
  list.add_line("1;");
  while (1)
    lex(loc, list);
}
