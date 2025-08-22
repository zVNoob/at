#pragma once

#include <string>

namespace token {
#pragma once

// Type definition
// 0: Code
// 1: Expression
// 2: Function
// 3: Array
// 4: Index
// 5: String
// 6: Operator
// 7: Identifier
// 8: Integer
// 9: Decimal
// 10: Special

struct Token {
	unsigned char Type;
  std::string Value;
	Token *PreExec;
	Token(std::string Value, unsigned char Type, Token *PreExec = 0)
		: Value(Value), Type(Type), PreExec(PreExec){};
	Token *Next = 0;
	Token *Prev = 0;

	Token *ChildBegin = 0;
	Token *ChildEnd = 0;

	Token *Parent = 0;

	~Token() {
		detach();
		while (ChildBegin) {
			Token *temp = ChildBegin;
			ChildBegin = ChildBegin->Next;
			delete temp;
		}
		if (PreExec) {
			delete PreExec;
		}
	}

	Token *clone() {
		Token *Result = new Token(Value, Type);
		// Clone child tokens
		Token *Iterator = ChildBegin;
		while (Iterator) {
			*Result < Iterator->clone();
			Iterator = Iterator->Next;
		}
		// clone pre-exec
		if (PreExec)
			Result->PreExec = PreExec->clone();
		return Result;
	}

	inline Token &operator<(Token *inp) {
		if (inp) {
			if (ChildEnd)
				ChildEnd->Next = inp;
			else
				ChildBegin = inp;
			inp->Parent = this;
			inp->Prev = ChildEnd;
			ChildEnd = inp;
		}
		return *this;
	}

	inline Token *detach() {
		if (Next)
			Next->Prev = Prev;
		if (Prev)
			Prev->Next = Next;
		if (Parent) {
			if (Parent->ChildBegin == this)
				Parent->ChildBegin = Next;
			if (Parent->ChildEnd == this)
				Parent->ChildEnd = Prev;
		}
		Next = Prev = Parent = 0;
		return this;
	}
};


}
