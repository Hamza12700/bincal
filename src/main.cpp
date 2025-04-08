#include <sys/wait.h>

#include "file_system.cpp"
#include "parser.cpp"

#define Asmfile "asmfile.asm"
#define Asmbin "./asmfile"

int main() {
   auto allocator = fixed_allocator(getpagesize() * 2);
   bool show_asm = false; // Option to display the generated assembly

   while (true) {
      allocator.reset();

      auto buffer = string_with_size(&allocator, 1000);
      auto lhs = string_with_size(&allocator, 1000);
      auto rhs = string_with_size(&allocator, 1000);

      write(STDERR_FILENO, "> ", 2);
      read(STDIN_FILENO, buffer.buf, buffer.capacity-1);

      if (buffer[0] == '\n') continue; // Skip the empty expression

      uint buflen = buffer.len();

      // Remove the newline character from the end of the expression
      if (buffer[buflen-1] == '\n') {
         buffer.remove(buflen-1);
         buflen -= 1;
      }

      if (buflen <= 2) {
         printf("Expression is too short or invalid!\n");
         continue;
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

      if (!parse_input(&buffer, &allocator)) continue;

      // Sucks, that 'atol, atoi, ...' doesn't give a good error message.
      // I may have to implement a function for parsing integers, floats and etc myself.
      // But this is not important yet, maybe for more complex expressions like having operator precedence.

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
