#include "lexer.cpp"

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

Btree *parse_expression(Lexer lexer) {
   auto allocator = fixed_allocator(getpagesize()*2);

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
         auto root = static_cast <Btree *> (allocator.alloc(sizeof(Btree)));
         auto right = static_cast <Btree *> (allocator.alloc(sizeof(Btree)));

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
         auto root = static_cast <Btree *> (allocator.alloc(sizeof(Btree)));
         auto left = static_cast <Btree *> (allocator.alloc(sizeof(Btree)));
         auto right = static_cast <Btree *> (allocator.alloc(sizeof(Btree)));

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

   return last;
};
