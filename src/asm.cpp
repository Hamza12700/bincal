#include "parser.cpp"

struct Asm_Writer {
   FILE *file;
   Asm_Writer(char *filename);
   ~Asm_Writer();

   template<typename ...Args>
   void write(char *buf, Args ...args);
   void write(String &buf);

   void rax_mov_add(int num1, int num2); // 'mov' the number into 'rax' register and then perform the add operation
   void rax_add(int num); // perform the 'add' operation on 'rax' register

   void rax_mov_sub(int num1, int num2);
   void rax_sub(int num);
};

Asm_Writer::Asm_Writer(char *filename) {
   file = fopen(filename, "w");
   assert_err(!file, "failed to open file");

   fprintf(file, "format ELF64 executable\n");
   fprintf(file, "entry start\n\n");

   fprintf(file, "start:\n");
}

Asm_Writer::~Asm_Writer() {
   fprintf(file, "\n");
   fprintf(file, "   mov eax, 60\n");
   fprintf(file, "   mov ebx, 0\n");
   fprintf(file, "   syscall\n");
}

void Asm_Writer::write(String &buf) {
   fprintf(file, "   %s\n", buf.buf);
}

template<typename ...Args>
void Asm_Writer::write(char *buf, Args ...args) {
   auto buffer = format_string(buf, args...);
   fprnitf(file, "   %s\n", buffer.buf); 
}

void Asm_Writer::rax_mov_add(int num1, int num2) {
   fprintf(file, "   mov rax, %d\n", num1);
   fprintf(file, "   add rax, %d\n", num2);
}

void Asm_Writer::rax_add(int num) {
   fprintf(file, "   add rax, %d\n", num);
}

void Asm_Writer::rax_sub(int num) {
   fprintf(file, "   sub rax, %d\n", num);
}

void Asm_Writer::rax_mov_sub(int num1, int num2) {
   fprintf(file, "   mov rax, %d\n", num1);
   fprintf(file, "   sub rax, %d\n", num2);
}

auto asm_writer = Asm_Writer("asm");

FILE *gen_asm(Btree *tree) {
   if (tree == NULL) return NULL; 
   if (tree->left) gen_asm(tree->left);
   if (!tree->parent) return NULL;

   auto parent = tree->parent;

   char op = parent->op;
   auto left = parent->left;
   auto right = parent->right;

   if (op == '+') {
      if (left->val != 0)
         asm_writer.rax_mov_add(left->val, right->val);
      else
         asm_writer.rax_add(right->val);
   } else if (op == '-') {
      if (left->val != 0)
         asm_writer.rax_mov_sub(left->val, right->val);
      else
         asm_writer.rax_sub(right->val);
   }

   return asm_writer.file;
}
