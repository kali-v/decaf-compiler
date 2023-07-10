#ifndef decaf_HPP
#define decaf_HPP

extern int lineno;
extern int colno;
extern int matchLen;
extern std::string token;
extern std::string value;

enum TokenId { T_CERROR = 97, T_WERROR = 98, T_ERRORS = 99 };

#endif