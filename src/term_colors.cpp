#ifndef COLORS_H
#define COLROS_H

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

//#define red(input) "\033[31;5;196m" STRINGIFY(input) "\033[0m"
#define red(input) "\x1b[31m" STRINGIFY(input) "\033[0m"

#endif // COLORS_H
