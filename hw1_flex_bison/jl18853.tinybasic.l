%{
#include "jl18853.tinybasic.tab.h"
%}

%option noyywrap yylineno nodefault

%%

"PRINT"     { return PRINT; }
"IF"        { return IF; }
"THEN"      { return THEN; }
"GOTO"      { return GOTO; }
"INPUT"     { return INPUT; }
"LET"       { return LET; }
"GOSUB"     { return GOSUB; }
"RETURN"    { return RETURN; }
"CLEAR"     { return CLEAR; }
"LIST"      { return LIST; }
"RUN"       { return RUN; }
"END"       { return END; }

[A-Z]       { return VAR; }
[A-Za-z_][A-Za-z0-9_]* { return '?'; }

[0-9]       { return DIGIT; }
\"([^"\\\r\n]|\\[^\r\n])*\" { return STRING; }
","         { return COMMA; }
[+-]        { return PLUMIN; }
[*/]        { return MULDIV; }
"("         { return LPAREN; }
")"         { return RPAREN; }
"<>"|"<="|">="|"<"|">" { return RELOP; }
"="         { return '='; }

\r\n|\n|\r  { return CR; }
[ \t]+      { }
.           { return '?'; }

%%
