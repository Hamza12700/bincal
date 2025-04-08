#include "array.cpp"
#include "strings.cpp"

enum Token : u8 {
   Number,
   Alphabet,
   //Whitespace, Not sure if I need this?

   Equal,
   Minus,
   Plus,
   Asterisk,
   Forward_Slash,
   Percent,      // '%'
   Ampersand,

   Greater_Than, // '>'
   Less_Than,    // '<'

   Open_Paren,   // '('
   Close_Paren,  // ')'

   Unknown,      // Any character/symbol that isn't listed in this enum gets this
};

Array<Token> token_stream(const String *string, Fixed_Allocator *allocator) {
   // Token stream can't be larger than the original input stream
   auto tokens = Array<Token> (allocator, string->len());
   
   for (uint i = 0; i < string->len(); i++) {
      const char c = (*string)[i];

      if (c == ' ') continue; // Skip the whitespace

      if (isdigit(c)) {
         tokens.push(Number);
         continue;
      }

      if (isalpha(c)) {
         tokens.push(Alphabet);
         continue;
      }

      if (c == '%') {
         tokens.push(Percent);
         continue;
      }

      if (c == '/') {
         tokens.push(Forward_Slash);
         continue;
      }

      if (c == '=') {
         tokens.push(Equal);
         continue;
      }

      if (c == '+') {
         tokens.push(Plus);
         continue;
      }

      if (c == '-') {
         tokens.push(Minus);
         continue;
      }

      if (c == '*') {
         tokens.push(Asterisk);
         continue;
      }

      if (c == '&') {
         tokens.push(Ampersand);
         continue;
      }

      if (c == '>') {
         tokens.push(Greater_Than);
         continue;
      }

      if (c == '<') {
         tokens.push(Less_Than);
         continue;
      }

      if (c == '(') {
         tokens.push(Open_Paren);
         continue;
      }

      if (c == ')') {
         tokens.push(Close_Paren);
         continue;
      }

      // Everything else gets the 'Unknown' Token type
      tokens.push(Unknown);
      continue;
   }

   return tokens;
}
