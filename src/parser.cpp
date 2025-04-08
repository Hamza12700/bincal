#include "lexer.cpp"

bool is_operator(const Token token) {
   switch (token) {
      case Plus:
      case Minus:
      case Asterisk:
      case Forward_Slash: return true;

      default: return false;
   }
}

// @Temporary:
//
// This should return some kind-of expression Type which will then be used to generate the x86_64-bit assembly.
// But for now we are returning a boolean value to check for failures.

bool parse_input(const String *input, Fixed_Allocator *allocator) {
   auto tokens = token_stream(input, allocator);

   // @Temporary: For now, every expression starts with a number (positive or negative)
   if (tokens[0] == Plus || tokens[0] == Minus) {
      if (tokens[1] != Number) { // The next token should be a number
         printf("\nInvalid Expression:\n");
         printf("\t'+' and '-' can't operator on non-numeric values\n");
         return false;
      }

   } else if (tokens[0] != Number) { // If doesn't start with plus/minus signs then it should be a number
      printf("\nExpression doesn't start with a numerical value!\n");
      return false;
   }

   for (uint i = 0; i < tokens.index; i++) {
      auto token = tokens[i];

      if (token == Plus && i > 0) {
         if (tokens[i-1] != Number) {
            printf("\n[Error]: '+' can't operator on non-numerical value\n");

            printf("%s\n", input->buf);
            for (uint x = 1; x < i; x++) write(STDOUT_FILENO, "-", 1);
            printf("^ isn't a numerical value\n");
            return false;
         }

         if (i+1 >= tokens.capacity) {
            printf("\b[Error]: Expected a numerical value after the operator\n");

            printf("%s\n", input->buf);
            for (uint x = 0; x < i; x++) write(STDOUT_FILENO, "-", 1);
            printf(" ^ expected a numerical value here\n");
            return false;
         }

         auto next = tokens[i+1];
         if (next != Number) {
            printf("\nInvalid Expression:\n");

            printf("%s\n", input->buf);
            for (uint x = 0; x < i; x++) write(STDOUT_FILENO, "-", 1);
            printf("^ expected a numerical value here\n");
            return false;
         }
      }
   }

   return true;
}
