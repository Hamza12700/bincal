#include "lexer.cpp"

bool is_operator(const Token token) {
   switch (token) {
      case Plus:
      case Minus:
      case Equal:
      case Percent:
      case Asterisk:
      case Forward_Slash: return true;

      default: return false;
   }
}

char operator_name(const Token op) {
   switch (op) {
      case Plus: return '+';
      case Minus: return '-';
      case Equal: return '=';
      case Percent: return '%';
      case Asterisk: return '*';
      case Forward_Slash: return '/';

      default: return 0;
   }
}

// Helper function to check for errors when using an operator
static bool operator_validation(Array<Token> &tokens, const int token_idx, const char *source) {
   if (!is_operator(tokens[token_idx])) {
      printf("Not an operator\n");
      printf("Token-index: %u, source: %s\n", token_idx, source);
      return false;
   }

   if (token_idx-1 < 0) {
      printf("\nLeft side of the expression has no value\n");
      return false;
   }

   if (tokens[token_idx-1] != Number) {
      printf("\n[Error]: can't operator on non-numerical value\n");

      printf("%s\n", source);
      for (int x = 1; x < token_idx; x++) write(STDOUT_FILENO, "-", 1);
      printf("^ isn't a number\n");
      return false;
   }

   if (token_idx+1 >= tokens.capacity) {
      printf("\n[Error]: Expected a number after the operator\n");

      printf("%s\n", source);
      for (int x = 0; x < token_idx; x++) write(STDOUT_FILENO, "-", 1);
      printf(" ^ expected a number here\n");
      return false;
   }

   auto next = tokens[token_idx+1];
   if (next != Number) {
      printf("\nInvalid Expression:\n");

      printf("%s\n", source);
      for (int x = 0; x <= token_idx; x++) write(STDOUT_FILENO, "-", 1);
      printf("^ expected a number here\n");
      return false;
   }

   return true;
}

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
