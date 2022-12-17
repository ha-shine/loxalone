# Loxalone

Loxalone is the educational lox interpreter implementation from the book 
[Crafting Compilers](https://craftinginterpreters.com/). The commit history can be messy in this one since I was
on the go with multiple devices when I was reading the book and committing frequently for bookmarking.

# Grammar

## Precedence and associativity

Lowest to highest

| Name       | Operator          | Associates |
|------------|-------------------|------------|
| Equality   | `==` `!=`         | Left       |
| Comparison | `>` `>=` `<` `<=` | Left       |
| Term       | `-` `+`           | Left       |
| Factor     | `/` `*`           | Left       |
| Unary      | `!` `-`           | Right      |

## Parser rule table

### Simple rule table

```
- expression -> literal
              | unary
              | binary
              | grouping 
              | assignment ;
- assignment -> IDENTIFIER "=" assignment 
              | equality;
- literal    -> NUMBER | STRING | "true" | "false" | "nil" ;
- grouping   -> "(" expression ")" ;
- unary      -> ( "-" | "!" ) expression ;
- binary     -> expression operator expression ;
- operator   -> "==" | "!=" | "<" | "<=" | ">" | ">="
              | "+" | "-"  | "*" | "/" ;
```

### Parser rule table

Each rule matches expression at its precedence level or higher.
The rules are made intentionally to be right-recursive.
Terminals are in capital letters.

```

- program           -> declaration* EOF_ ;
- declaration       -> fun_decl 
                     | var_decl
                     | statement ;
- fun_decl          -> "fun" function ;
- function          -> IDENTIFIER "(" parameters? ")" block ;
- parameters        -> IDENTIFIER ( "," IDENTIFIER )* ;
- var_decl          -> "var" IDENTIFIER ( "=" expression )? ";" ;
- statement         -> expr_statement
                     | for_statement
                     | if_statement
                     | print_statement
                     | while_statement 
                     | block ;
- for_statement     -> "for" "(" ( var_decl | expr_statement | ";" )
                       expression? ";"
                       expression? ")" statement ;
- if_statement      -> "if" "(" expression ")" statement
                       ( "else" statement )? ;
- block             -> "{" declaration* "}" ;
- expr_statement    -> expression ";" ;
- print_statement   -> "print" expression ";" ;
- while_statement   -> "while" "(" expression ")" statement ;
- expression        -> assignment ;
- assignment        -> IDENTIFIER "=" assignment
                     | logic_or ;
- logic_or          -> logic_and ( "or" logic_and )* ;
- logic_and         -> equality ( "and" equality )* ;
- equality          -> comparison ( ("==" | "!=") comparison )*;
- comparison        -> term ( (">" | ">=" | "<" | "<=") term )*;
- term              -> factor ( ("+" | "-") factor )*;
- factor            -> unary ( ("/" | "*") unary )*;
- unary             -> ("!" | "-") unary | call ;
- call              -> primary ( "(" arguments? ")" )* ;
- arguments         -> expression ( "," expression )* ;
- primary           -> NUMBER | STRING | "true" | "false" | "nil"
                     | "(" expression ")"
                     | IDENTIFIER ;

```

### Feature Ideas (and challenges)

- Add support for C-style ternary operator (?:), one-liner if clauses
- `--verbose` parameter in CLI prompt for extra statistics (like execution time, etc.)
- Handle divide by 0 error
- `+` operator currently only concatenates or add numbers. Extend it to allow concatenating a number to a string
- REPL no longer supports entering single expression, it can be convenient for the user to be able to type expressions
  and see the results
- Add `break` and `continue` statement inside loops