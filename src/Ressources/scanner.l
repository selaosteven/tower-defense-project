%option c++
%option noyywrap
%option nodefault

%{
#include "parser.tab.hh"
using namespace yy;
%}

DIGIT   [0-9]
ID      [a-zA-Z_][a-zA-Z0-9_]*
WS      [ \t\r\n]+

%%
{DIGIT}+ {
    yylval->emplace<int>(std::stoi(yytext));
    return Parser::token::NUMBER;
}

{ID} {
    yylval->emplace<std::string>(yytext);
    return Parser::token::IDENT;
}

"+"     { return Parser::token::PLUS; }
"-"     { return Parser::token::MINUS; }
"*"     { return Parser::token::MUL; }
"/"     { return Parser::token::DIV; }

"("     { return Parser::token::LPAREN; }
")"     { return Parser::token::RPAREN; }

";"     { return Parser::token::END; }

{WS}    { /* ignorer */ }

.       { std::cerr << "Caractère inconnu : " << yytext << "\n"; }
%%