#include "lexer.cpp"

// @Temporary:
//
// This should return some kind-of expression Type which will then be used to generate the x86_64-bit assembly.
// But for now we are returning a boolean value to check for failures.

static bool parse_token(Array<Token> &tokens, const uint token_idx, const char *source) {
   const auto token = tokens[token_idx];

   switch (token) {
      case Number: return true;

      case Plus: {
         if (!operator_validation(tokens, token_idx, source))
            return false;

         return true;
      }

      case Minus: {
         if (!operator_validation(tokens, token_idx, source))
            return false;

         return true;
      }

      case Asterisk: {
         if (!operator_validation(tokens, token_idx, source))
            return false;

         return true;
      }

      case Forward_Slash: {
         if (!operator_validation(tokens, token_idx, source))
            return false;

         return true;
      }

      case Percent: {
         if (!operator_validation(tokens, token_idx, source))
            return false;

         return true;
      }

      case Equal: { // Compare two numerical values
         if (!operator_validation(tokens, token_idx, source))
            return false;

         return true;
      }

      case Greater_Than: {
         if (!operator_validation(tokens, token_idx, source))
            return false;

         return true;
      }

      case Less_Than: {
         if (!operator_validation(tokens, token_idx, source))
            return false;

         return true;
      }

      case Alphabet: {
         printf("\nUsing variables to store the result of an expression is not currently supported\n");
         return false;
      }

      case Open_Paren: {}
      case Close_Paren: {}

      case Unknown: {
         printf("\n[Error]: Unknown character encounted\n");

         printf("%s\n", source);
         for (uint x = 0; x < token_idx; x++) write(STDOUT_FILENO, "-", 1);
         write(STDOUT_FILENO, "^", 1); 

         return false;
      }
   }
}

static bool parse_input(const String *input, Fixed_Allocator *allocator) {
   auto tokens = token_stream(input, allocator);

   // @Temporary: For now, every expression starts with a number
   if (tokens[0] != Number) {
      printf("\nExpression doesn't start with a number!\n");
      return false;
   }

   for (uint i = 0; i < tokens.index; i++) {
      if (parse_token(tokens, i, input->buf)) continue;
      return false;
   }

   return true;
}
