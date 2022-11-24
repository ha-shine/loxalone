# Loxalone

Loxalone is the educatinal lox interpreter implementation from the book 
[Crafting Compilers](https://craftinginterpreters.com/). The commit history can be messy in this one since I was
on the go with multiple devices when I was reading the book and committing frequently for bookmarking.

# Grammar

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