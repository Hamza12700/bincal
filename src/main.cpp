#include "asm.cpp"

int main() {
   auto allocator = fixed_allocator(getpagesize() * 2);

   while (true) {
      allocator.reset();
      auto buffer = string_with_size(&allocator, 1000);

      write(STDIN_FILENO, "> ", 2);
      read(STDIN_FILENO, buffer.buf, buffer.capacity-1);

      if (buffer[0] == '\n') continue; // Skip if newline
      uint buflen = buffer.len();

      // Remove the newline character from the end of the expression
      if (buffer[buflen-1] == '\n') {
         buffer.remove(buflen-1);
         buflen -= 1;
      }

      if (buffer.cmp("quit")) {
         printf("Goodbye\n");
         break;
      }

      if (buffer.cmp("clear")) {
         printf("\x1b[2J\x1b[H\n");
         continue;
      }

      auto lexer = lex(buffer, &allocator);
      Btree *tree = parse_expression(lexer);

      if (!tree) continue;

      printf("\n--- START ---\n");
      gen_asm(tree);
      printf("--- END ---\n\n");
   }

   allocator.free();
}
