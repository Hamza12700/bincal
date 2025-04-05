#include <sys/wait.h>

#include "file_system.cpp"

#define Asmfile "asmfile.asm"
#define Asmbin "./asmfile"

enum Operator : u8 {
   Plus,
   Minus,
   Multiply,
   Divide,
   Unknown
};

Operator get_operator(char op) {
   switch (op) {
      case '+': return Plus;
      case '-': return Minus;
      case '*': return Multiply;
      case '/': return Divide;

      default: return Unknown;
   };
}

bool is_operator (char op) {
   if (get_operator(op) == Unknown) return false;
   return true;
}

FILE* generate_asm(const int num1, const int num2, const Operator op) {
   auto asmfile = fopen(Asmfile, "w+");
   assert_err(!asmfile, "tmpfile failed");

   fprintf(asmfile, "format ELF64 executable\n"); // Writing the header
   fprintf(asmfile, "_start:\n"); // The start section of the executable

   switch (op) {
      case Plus: {
         fprintf(asmfile, "\tmov rax, %d\n", num1);
         fprintf(asmfile, "\tadd rax, %d\n", num2);
         break;
      }

      case Minus: {
         fprintf(asmfile, "\tmov rax, %d\n", num1);
         fprintf(asmfile, "\tsub rax, %d\n", num2);
         break;
      }

      // @Temporary: Only for signed integers
      case Multiply: {
         fprintf(asmfile, "\tmov rax, %d\n", num1);
         fprintf(asmfile, "\tmov rbx, %d\n", num2);
         fprintf(asmfile, "\timul rax, rbx\n");
         break;
      }

      // @Temporary: Only for signed integers
      case Divide: {
         fprintf(asmfile, "\tmov rax, %d\n", num1);
         fprintf(asmfile, "\tmov rbx, %d\n", num2);
         fprintf(asmfile, "\tcqo\n"); // sign-extend rax into rdx (prepares rdx:rax for division)
         fprintf(asmfile, "\tidiv rbx\n"); // rax = rax / rbx
         break;
      }

      case Unknown: {
         assert(true, "Shouldn't get to unknown operator when generating asm");
         break;
      }
   }

   fprintf(asmfile, "\n");

   // Use the number stored in the 'eax' as an exit code!
   fprintf(asmfile, "\tmov rdi, rax\n");  // status code
   fprintf(asmfile, "\tmov rax, 60\n");   // exit syscall
   fprintf(asmfile, "\tsyscall\n");

   return asmfile;
}

void compile_asm(Fixed_Allocator *allocator) {
   fprintf(stderr, "Compiling... ");
   auto cmd = format_string(allocator, "./bin/fasm %", Asmfile);
   system(cmd.buf);
};

// Returns true on success otherwise false
bool execute_asm() {
   int pid = fork();
   if (pid == -1) {
      fprintf(stderr, "failed to fork the process");
      return false;
   }

   if (pid == 0) execl(Asmbin, Asmbin, NULL);

   int status;
   waitpid(pid, &status, 0);

   int exit_code = WEXITSTATUS(status);
   printf("Result: %d\n", exit_code);

   return true;
}

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

      bool is_valid = true;
      bool contains_operator = false;

      for (uint i = 0; i < buflen; i++) {
         char c = buffer[i];

         if (isspace(c)) continue; // Skip whitespace

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

      // @NOTE | @Temporary:
      // For I don't check for floating-point numbers.
      for (uint i = 0; i < buflen; i++) {
         while (!is_operator(buffer[i])) {
            if (isspace(buffer[i])) { // Skip whitespace
               i += 1;
               continue;
            }

            lhs[i] = buffer[i];
            i += 1;
         }

         op = get_operator(buffer[i]);
         i += 1; // Skip the operator

         uint x = 0;
         while (x < buflen) {
            if (isspace(buffer[i])) { // Skip whitespace
               i += 1;
               continue;
            }

            rhs[x] = buffer[i];

            x += 1;
            i += 1;
         }

         break; // @Temporary: Don't parse the rest of the expression
      }

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

      FILE *asmfile = generate_asm(lhs_num, rhs_num, op);
      auto contents = fread_into_string(&allocator, asmfile);

      if (show_asm) {
         printf("\nGenerated Assembly:\n\n");
         printf("%s", contents.buf);
         printf("\n--- END ---\n\n");
      }

      compile_asm(&allocator);
      execute_asm();

      // Remove generated files
      fclose(asmfile);
      unlink(Asmfile);
      unlink(Asmbin);
   }

   allocator.free();
}
