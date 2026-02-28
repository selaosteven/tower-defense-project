%language "c++"
%define api.namespace {yy}
%define api.parser.class {Parser}
%define api.value.type variant
%define api.token.constructor
%define api.token.prefix {TOK_}

%code requires {
    #include <string>
}

%code {
    #include <iostream>
}

%token <int>       NUMBER
%token <std::string> IDENT
%token PLUS MINUS MUL DIV
%token LPAREN RPAREN
%token END

%type <int> expression

%start input

%%
input:
      /* vide */
    | input expression END   { std::cout << "Résultat = " << $2 << "\n"; }
    ;

expression:
      NUMBER                 { $$ = $1; }
    | expression PLUS expression   { $$ = $1 + $3; }
    | expression MINUS expression  { $$ = $1 - $3; }
    | expression MUL expression    { $$ = $1 * $3; }
    | expression DIV expression    { $$ = $1 / $3; }
    | LPAREN expression RPAREN     { $$ = $2; }
    ;
%%
void yy::Parser::error(const std::string& msg) {
    std::cerr << "Erreur syntaxique : " << msg << "\n";
}