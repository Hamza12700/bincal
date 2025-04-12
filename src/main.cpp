#include "lexer.cpp"

int main() {
   auto allocator = fixed_allocator(getpagesize() * 2);
   bool show_asm = false; // Option to display the generated assembly

   while (true) {
      allocator.reset();
      auto buffer = string_with_size(&allocator, 1000);

      write(STDERR_FILENO, "> ", 2);
      read(STDIN_FILENO, buffer.buf, buffer.capacity-1);

      if (buffer[0] == '\n') continue; // Skip the empty expression

      uint buflen = buffer.len();

      // Remove the newline character from the end of the expression
      if (buffer[buflen-1] == '\n') {
         buffer.remove(buflen-1);
         buflen -= 1;
      }

      // @Temporary: Hardcoding the commands name and their functionality for now

      if (buffer.cmp("quit") || buffer.cmp("exit")) {
         printf("Goodbye\n");
         break;
      }

      if (buffer.cmp("clear")) {
         printf("\x1b[2J\x1b[H\n");
         continue;
      }

      if (buffer.cmp("show asm")) {
         show_asm = true;
         continue;
      }

      if (buffer.cmp("hide asm")) {
         show_asm = false;
         continue;
      }

      if (!lexer(&buffer, &allocator)) continue;
   }

   allocator.free();
}
