#include "parser.cpp"

struct Asm_Writer {
   template<typename ...Args>
   void write(char *buf, Args ...args);
   void write(String &buf);

   void rax_mov_add(int num1, int num2); // 'mov' the number into 'rax' register and then perform the add operation
   void rax_add(int num); // perform the 'add' operation on 'rax' register

   void rax_mov_sub(int num1, int num2);
   void rax_sub(int num);
};

void Asm_Writer::write(String &buf) {
   printf("%s\n", buf.buf);
}

template<typename ...Args>
void Asm_Writer::write(char *buf, Args ...args) {
   auto buffer = format_string(buf, args...);
   printf("%s\n", buffer.buf); 
}

void Asm_Writer::rax_mov_add(int num1, int num2) {
   printf("mov rax, %d\n", num1);
   printf("add rax, %d\n\n", num2);
}

void Asm_Writer::rax_add(int num) {
   printf("add rax, %d\n", num);
}

void Asm_Writer::rax_sub(int num) {
   printf("sub rax, %d\n", num);
}

void Asm_Writer::rax_mov_sub(int num1, int num2) {
   printf("mov rax, %d\n", num1);
   printf("sub rax, %d\n\n", num2);
}

Asm_Writer asm_writer;

void gen_asm(Btree *tree) {
   if (tree == NULL) return; 
   if (tree->left) gen_asm(tree->left);
   if (!tree->parent) return;

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

   return;
}
