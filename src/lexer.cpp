#include "array.cpp"
#include "strings.cpp"
#include "term_colors.cpp"

struct BTree; // Binary-Tree for math operators

struct Token;

struct BTree {
   size_t value;

   BTree *left = NULL;
   BTree *right = NULL;

   char op;
};

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

struct Token {
   uint idx = 0; // Index to the token where it is, so we can point to it without caring about whitespace.

   char value;
   Token_Type type = Unknown;
};

bool lexer(const String *string, Fixed_Allocator *allocator) {

   auto tokens = Array<Token> (allocator, string->len()); // Token stream can't be larger than the original input string in length
   
   for (uint i = 0; i < string->len(); i++) {
      const char c = (*string)[i];
      Token token;

      if (c == ' ') continue; // Skip the whitespace. We don't need it because we keep the track of the character index

      if (isdigit(c)) {
         token.type = Number;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (isalpha(c)) {
         token.type = Alphabet;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '%') {
         token.type = Percent;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '/') {
         token.type = Forward_Slash;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '=') {
         token.type = Equal;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '+') {
         token.type = Plus;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '-') {
         token.type = Minus;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '*') {
         token.type = Asterisk;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '>') {
         token.type = Greater_Than;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '<') {
         token.type = Less_Than;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == '(') {
         token.type = Open_Paren;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      if (c == ')') {
         token.type = Close_Paren;
         token.idx = i;
         token.value = c;

         tokens.push(token);
         continue;
      }

      // Everything else gets the 'Unknown' Token type
      token.type = Unknown;
      token.value = c;
      token.idx = i;

      tokens.push(token);
      continue;
   }

   for (uint i = 0; i < tokens.index; i++) {
      auto token = tokens[i]; 

      if (token.type == Alphabet) {
         fprintf(stderr, "\n[Error]: Expression can not contain any alphabet characters\n");
         fprintf(stderr, "%s\n", string->buf);

         if (token.idx > 0)
            fprintf(stderr, "\033[%uC", token.idx); // Move the terminal cursor at the token index

         for (uint x = token.idx; x < string->len(); x++) {
            auto next = (*string)[x];
            if (isascii(next)) {
               fprintf(stderr, "%s", red(^));
               continue;
            }

            break;
         }

         write(STDERR_FILENO, "\n", 1);
         return false;
      }

      if (token.type == Plus) {
      }
   }

   return true;
}
