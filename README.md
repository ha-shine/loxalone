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
- expression -> equality;
- equality   -> comparison ( ("==" | "!=") comparison )*;
- comparison -> term ( (">" | ">=" | "<" | "<=") term )*;
- term       -> factor ( ("+" | "-") factor )*;
- factor     -> unary ( ("/" | "*") unary )*;
- unary      -> ("!" | "-") unary | primary;
- primary    -> NUMBER | STRING | "true" | "false" | "nil"
              | "(" expression ")";
```