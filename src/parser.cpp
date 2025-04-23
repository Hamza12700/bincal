#include "lexer.cpp"

enum Operator_Precedence {
   Multiply = 10,
   Divide = 5,
   Plus = 3,
   Minus = 0,
};

int get_operator_precedn(char op) {
   switch (op) {
      case '-': return Minus;
      case '+': return Plus;
      case '/': return Divide;
      case '*': return Multiply;
   }
   
   return -1;
}

void swap(int &num1, int &num2) {
   auto tmp = num1;
   num1 = num2;
   num2 = tmp;
}

void swap(char &one, char &two) {
   auto tmp = one;
   one = two;
   two = tmp;
}

struct Btree {
   Btree *left = NULL;
   Btree *right = NULL;
   Btree *parent = NULL;

   int val = 0;
   char op;
};

void print_tree(Btree *node) {
   if (node == NULL) return;

   if (node->val == 0)
      printf("operator: %c\n", node->op);
   else
      printf("num: %d\n", node->val);

   print_tree(node->left);
   print_tree(node->right);
}

void fix_precedn(Btree *tree) {
   if (!tree) return; 
   if (tree->left) fix_precedn(tree->left);
   if (!tree->parent || !tree->parent->parent) return;

   auto parent = tree->parent;
   auto grand_parent = parent->parent;

   auto parent_precedn = get_operator_precedn(parent->op);
   auto grand_parent_precedn = get_operator_precedn(grand_parent->op);

   printf("\n-- Original --\n");

   printf("parent: left: %d, right: %d\n", parent->left->val, parent->right->val);
   printf("grand-parent: left: %d, right: %d\n", grand_parent->left->val, grand_parent->right->val);
   printf("---- ---- ----\n");

   if (grand_parent_precedn > parent_precedn) {
      printf("\n-- swapping --\n");

      swap(parent->op, grand_parent->op);
      swap(parent->right->val, parent->left->val);
      swap(parent->right->val, grand_parent->right->val);
   }

   parent_precedn = get_operator_precedn(parent->op);
   grand_parent_precedn = get_operator_precedn(grand_parent->op);

   printf("parent: left: %d, right: %d\n", parent->left->val, parent->right->val);
   printf("grand-parent: left: %d, right: %d\n", grand_parent->left->val, grand_parent->right->val);
   printf("---- ---- ----\n");
}

auto parser_alloc = fixed_allocator(getpagesize()*2);

Btree *parse_expression(Lexer lexer) {
   Btree *last = NULL;
   auto tokens = lexer.tokens;

   for (auto token = lexer.next_token(); token; token = lexer.next_token()) {
      if (isalpha(token->literal)) { // @Temporary: Error case for now
         fprintf(stderr, "\n[Error]: Expression can not contain any alphabet characters\n");
         return NULL;
      }

      if (!token->is_num && !is_operator(token->literal)) {
         fprintf(stderr, "Expression doesn't contain any numerical value\n");
         return NULL;
      }

      if (is_operator(token->literal)) {
         auto root = static_cast <Btree *> (parser_alloc.alloc(sizeof(Btree)));
         auto right = static_cast <Btree *> (parser_alloc.alloc(sizeof(Btree)));

         root->op = token->literal;
         auto next = lexer.next_token();
         if (!next->is_num) {
            fprintf(stderr, "\nExpected a number after the operator\n");
            return NULL;
         }

         right->val = next->num;
         root->right = right;
         right->parent = root;

         root->left = last;
         last->parent = root;

         last = root;
      }

      if (token->is_num) {
         auto root = static_cast <Btree *> (parser_alloc.alloc(sizeof(Btree)));
         auto left = static_cast <Btree *> (parser_alloc.alloc(sizeof(Btree)));
         auto right = static_cast <Btree *> (parser_alloc.alloc(sizeof(Btree)));

         left->val = token->num;

         auto *next = lexer.next_token();
         if (!next) return NULL;

         if (!is_operator(next->literal)) {
            fprintf(stderr, "\nExpected an operator after the number\n");
            return NULL;
         }

         root->op = next->literal;

         next = lexer.next_token();
         if (!next->is_num) {
            fprintf(stderr, "\nExpected a number after the operator\n");
            return NULL;
         }

         right->val = next->num;

         root->left = left;
         root->right = right;

         left->parent = root;
         right->parent = root;

         last = root;
      }
   }

   fix_precedn(last);
   return last;
};
