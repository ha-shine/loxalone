# Loxalone

Loxalone is the educatinal lox interpreter implementation from the book 
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
               | grouping ;
                
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

```

- program           -> statement* EOF ;
- statement         -> expr_statement
                     | print_statement ;
- expr_statement    -> expression ";" ;
- print_statement   -> "print" expression ";" ;
- expression        -> equality;
- equality          -> comparison ( ("==" | "!=") comparison )*;
- comparison        -> term ( (">" | ">=" | "<" | "<=") term )*;
- term              -> factor ( ("+" | "-") factor )*;
- factor            -> unary ( ("/" | "*") unary )*;
- unary             -> ("!" | "-") unary | primary;
- primary           -> NUMBER | STRING | "true" | "false" | "nil"
                     | "(" expression ")";

```

### Feature Ideas (and challenges)

- Add support for C-style ternary operator (?:), one-liner if clauses
- `--verbose` parameter in CLI prompt for extra statistics (like execution time, etc.)
- Handle divide by 0 error
- `+` operator currently only concatenates or add numbers. Extend it to allow concatenating a number to a string