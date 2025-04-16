#include "array.cpp"
#include "strings.cpp"
#include "term_colors.cpp"

/*
enum Token_Type {
   Number,
   Alphabet,

   Equal,
   Minus,
   Plus,
   Asterisk,
   Forward_Slash,
   Percent,      // '%'

   Greater_Than, // '>'
   Less_Than,    // '<'

   Open_Paren,   // '('
   Close_Paren,  // ')'

   Unknown,      // Any character/symbol that isn't listed in this enum gets this
};
*/

enum Btree_Flags {
   LEFT_LITERAL = 1,
   LEFT_BTREE = 2,

   RIGHT_LITERAL = 4,
   RIGHT_BTREE = 8,

   BTREE = 16,
   LITERAL = 32,
};

struct Btree { // Binary-Tree for Binary expressions (duh)

   union {
      struct {
         int lvalue;
         int rvalue;
      };

      struct {
         void *left_tree;
         void *right_tree;
      };
   };

   int flags = 0; // Type information about the left and right nodes
   char root;

   void push_left(void *tree_or_literal, int type);
};

// void Btree::push_left(void *tree_or_literal, int type) {
//    if (type & LITERAL) {
//       auto btree = static_cast <Btree *>(tree_or_literal);
//
//       root = btree->root;
//       flags = btree->flags;
//       left_tree = btree;
//       return;
//    }
//
//    auto literal = *static_cast <long *>(tree_or_literal);
// }

bool is_operator(char op) {
   switch (op) {
      case '+':
      case '-':
      case '=':
      case '*':
      case '/': return true;

      default: return false;
   }
}

struct Token {
   uint idx = 0; // Index to the token where it is in the string

   union { // @Temporary: I don't like this, I would like to do something more clean.
      int num;
      char literal;
   };

   bool is_num = false;
};

// Helper function to check for errors when using an operator
static bool validate_operator(Array<Token> &tokens, const int token_idx, const char *source) {
   if (!is_operator(tokens[token_idx].literal)) {
      printf("Not an operator\n");
      printf("Token-index: %u, source: %s\n", token_idx, source);
      return false;
   }

   if (token_idx-1 < 0) {
      printf("\nLeft side of the expression has no value\n");
      return false;
   }

   if (!isdigit(tokens[token_idx-1].literal)) {
      printf("\n[Error]: can't operator on non-numerical value\n");

      printf("%s\n", source);
      for (int x = 1; x < token_idx; x++) write(STDOUT_FILENO, "-", 1);
      printf("^ isn't a number\n");
      return false;
   }

   if (token_idx+1 >= tokens.capacity) {
      printf("\n[Error]: Expected a number after the operator\n");

      printf("%s\n", source);
      for (int x = 0; x < token_idx; x++) write(STDOUT_FILENO, "-", 1);
      printf(" ^ expected a number here\n");
      return false;
   }

   auto next = tokens[token_idx+1];
   if (!isdigit(next.literal)) {
      printf("\nInvalid Expression:\n");

      printf("%s\n", source);
      for (int x = 0; x <= token_idx; x++) write(STDOUT_FILENO, "-", 1);
      printf("^ expected a number here\n");
      return false;
   }

   return true;
}

bool lexer(const String *string, Fixed_Allocator *allocator) {

   auto tokens = Array<Token> (allocator, string->len()); // Token stream can't be larger than the original input string in length
   
   for (uint i = 0; i < string->len(); i++) {
      const char c = (*string)[i];
      Token token = {0};

      if (c == ' ') continue; // Skip the whitespace. We don't need it because we keep the track of the character index

      if (isdigit(c)) {
         token.idx = i; // Starting index
         auto dstring = *string;

         uint count = 0;
         for (uint x = i; x < string->len(); x++) {
            if (isdigit(dstring[x])) count += 1;
            else break;
         }

         auto buffer = string_with_size(allocator, count);
         while (isdigit(dstring[i])) {
            buffer.concat(dstring[i]);
            i += 1;
         }

         i -= 1; // Get back to previous character

         token.num = atoi(buffer.buf);
         token.is_num = true;

         tokens.push(token);
         continue;
      }

      if (isalpha(c)) {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '%') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '/') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '=') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '+') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '-') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '*') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '>') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '<') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == '(') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      if (c == ')') {
         token.literal = c;
         token.idx = i;

         tokens.push(token);
         continue;
      }

      // Everything else gets '0'
      token.literal = 0;
      token.idx = i;

      tokens.push(token);
      continue;
   }

   for (uint i = 0; i < tokens.index; i++) {
      auto token = tokens[i]; 

      if (isalpha(token.literal)) { // @Temporary: Error case for now
         fprintf(stderr, "\n[Error]: Expression can not contain any alphabet characters\n");
         fprintf(stderr, "%s\n", string->buf);

         if (token.idx > 0)
            fprintf(stderr, "\033[%uC", token.idx); // Move the terminal cursor at the token index

         for (uint x = token.idx; x < string->len(); x++) {
            auto next = (*string)[x];
            if (isalpha(next)) {
               fprintf(stderr, "%s", red(^));
               continue;
            }

            break;
         }

         write(STDERR_FILENO, "\n", 1);
         return false;
      }

      if (token.is_num) {
         Btree tree;

         tree.lvalue = token.num;

         i += 1;
         if (i > tokens.index) {
            fprintf(stderr, "Expected an operator\n");
            return false;
         }

         auto next = tokens[i];
         tree.root = next.literal;

         i += 1;
         if (i > tokens.index) {
            fprintf(stderr, "Expected a number\n");
            return false;
         }

         tree.rvalue = tokens[i].num;
      }

      /*
      if (token.type == '+') {
         if (!validate_operator(tokens, i, string->buf))
            return false;

         auto left = tokens[i-1];
         auto right = tokens[i+1];
         Btree btree;

         btree.left = &left;
         btree.right = &right;
         btree.root = token.type;

         btree.flags |= LEFT_LITERAL | RIGHT_LITERAL;
         continue;
      }

      if (token.type == '-') {
         if (!validate_operator(tokens, i, string->buf))
            return false;

         auto left = tokens[i-1];
         auto right = tokens[i+1];
         Btree btree;

         btree.left = &left;
         btree.right = &right;
         btree.root = token.type;

         btree.flags |= LEFT_LITERAL | RIGHT_LITERAL;
         continue;
      }

      if (token.type == '*') {
         if (!validate_operator(tokens, i, string->buf))
            return false;

         auto left = tokens[i-1];
         auto right = tokens[i+1];
         Btree btree;

         btree.left = &left;
         btree.right = &right;
         btree.root = token.type;

         btree.flags |= LEFT_LITERAL | RIGHT_LITERAL;
         continue;
      }

      if (token.type == '/') {
         if (!validate_operator(tokens, i, string->buf))
            return false;

         auto left = tokens[i-1];
         auto right = tokens[i+1];
         Btree btree;

         btree.left = &left;
         btree.right = &right;
         btree.root = token.type;

         btree.flags |= LEFT_LITERAL | RIGHT_LITERAL;
         continue;
      }
   */
   }

   return true;
}
