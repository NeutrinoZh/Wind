#pragma once
#include "Log.h"

namespace EngineCore {
	namespace JText {
		enum class TokenType {
			T_EOF,
			WORD,
			NUMBER,
			STRING,

			KEYWORD_CONST,
			KEYWORD_LINK,
			KEYWORD_AS,
			KEYWORD_INHERIT,

			L_BRACKET,
			R_BRACKET,

			L_CUR_BRACKET,
			R_CUR_BRACKET,
			
			L_SQRT_BRACKET,
			R_SQRT_BRACKET,

			EQ,
			COMMA,
			PERCENT
		};

		typedef std::pair<TokenType, std::string> Token;
	}
}