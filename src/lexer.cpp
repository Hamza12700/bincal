#include "array.cpp"
#include "strings.cpp"

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

struct Lexer {
   Array<Token> tokens;
   int index = -1;

   Token *get_token(uint idx);
   Token *next_token();
   Token *peek_next();
};

Token *Lexer::get_token(uint idx) {
   if (idx >= tokens.size) return NULL;
   return &tokens[idx];
}

Token *Lexer::next_token() {
   if (index+1 >= tokens.size) return NULL;
   index += 1;
   return &tokens[index];
}

Token *Lexer::peek_next() {
   if (index+1 >= tokens.size) return NULL;
   return &tokens[index+1];
}

Lexer lex(String &string, Fixed_Allocator *allocator) {
   Lexer lexer;
   lexer.tokens = make_array<Token> (allocator, string.len()); // Token stream can't be larger than the original input string in length
   
   for (uint i = 0; i < string.len(); i++) {
      char c = string[i];
      Token token;

      if (isdigit(c)) {
         token.idx = i; // Starting index of the number

         uint count = 0;
         for (uint x = i; x < string.len(); x++) {
            if (isdigit(string[x])) count += 1;
            else break;
         }

         auto buffer = string_with_size(allocator, count);
         while (isdigit(string[i])) {
            buffer.concat(string[i]);
            i += 1;
         }

         token.num = atoi(buffer.buf);
         token.is_num = true;

         lexer.tokens.push(token);

         // @NOTE | @Temporary:
         //
         // We have to go back one character because we want to continue the loop, even tho we know that the next character isn't a number,
         // we still have to do it because of the check in the loop 'i < string.len()'
         //
         // - Hamza 22 April 2025

         i -= 1;
         continue;
      }

      if (c == ' ') continue; // Skip the whitespace. We don't need it because we keep the track of the character index

      if (isalpha(c)) {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == '/') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == '=') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == '+') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == '-') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == '*') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == '>') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == '<') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == '(') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      if (c == ')') {
         token.literal = c;
         token.idx = i;

         lexer.tokens.push(token);
         continue;
      }

      token.literal = -1;
      token.idx = i;

      lexer.tokens.push(token);
      continue;
   }

   return lexer;
}
