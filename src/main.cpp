#include "strings.cpp"

enum Operator : u8 {
   Plus,
   Minus,
   Multiply,
   Devide,
   Unknown
};

Operator get_operator(char op) {
   switch (op) {
      case '+': return Plus;
      case '-': return Minus;
      case '*': return Multiply;
      case '/': return Devide;

      default: return Unknown;
   };
}

bool is_operator (char op) {
   if (get_operator(op) == Unknown) return false;
   return true;
}

int main() {
   auto allocator = fixed_allocator(1 << 12);

   auto buffer = string_with_size(&allocator, 1000);
   auto lhs = string_with_size(&allocator, 1000);
   auto rhs = string_with_size(&allocator, 1000);

   while (true) {
      memset(buffer.buf, 0, buffer.len()+1);
      memset(lhs.buf, 0, lhs.len()+1);
      memset(rhs.buf, 0, lhs.len()+1);

      write(STDERR_FILENO, "> ", 2);
      read(STDIN_FILENO, buffer.buf, buffer.capacity-1);

      uint buflen = buffer.len();
      if (buffer[buflen-1] == '\n') {
         buffer.remove(buflen-1);
         buflen -= 1;
      }

      if (buflen <= 1) {
         printf("Expression is too short or invalid!\n");
         continue;
      }

      if (buffer.cmp("quit") || buffer.cmp("exit")) {
         printf("Goodbye\n");
         break;
      }

      if (buffer.cmp("clear")) {
         printf("\x1b[2J\x1b[H\n");
         continue;
      }

      bool is_valid = true;
      bool contains_operator = false;

      for (uint i = 0; i < buflen; i++) {
         char c = buffer[i];

         if (isalpha(c)) {
            printf("\nExpression can not contain a alphabetical character\n");
            printf("%s\n", buffer.buf);

            for (uint x = 0; x < i; x++) printf("-");
            printf("^ contains an alphabetic charactre\n");
            is_valid = false;
            break;
         }

         if (is_operator(c)) contains_operator = true;

         // Handle unknown characters
         if (!isdigit(c) && !is_operator(c)) {
            printf("\nExpression contains a invalid/unknown character!\n");
            printf("%s\n", buffer.buf);

            for (uint x = 0; x < i; x++) printf("-"); 
            printf("^ unknown charactre\n");
            is_valid = false;
            break;
         }
      }

      if (!contains_operator) {
         printf("\nExpression doesn't contain any operator! (which is required)\n");
         continue;
      }

      if (!is_valid) continue;

      auto op = Unknown;
      for (uint i = 0; i < buflen; i++) {
         while (!is_operator(buffer[i])) {
            lhs[i] = buffer[i];
            i += 1;
         }

         op = get_operator(buffer[i]);
         i += 1; // Skip the operator

         uint x = 0;
         while (x < buflen) {
            rhs[x] = buffer[i];

            x += 1;
            i += 1;
         }

         break; // @Temporary: Don't parse the rest of the expression
      }

      int lhs_num = atoi(lhs.buf);
      if (lhs_num == 0) {
         printf("\n'atoi' returned 0 with this LHS value '%s'\n", lhs.buf);
         continue;
      }

      int rhs_num = atoi(rhs.buf);
      if (rhs_num == 0) {
         printf("\n'atoi' returned 0 with this RHS value '%s'\n", rhs.buf);
         continue;
      }
   }

   allocator.free();
}
