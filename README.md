# Neylang

neylang is a strongly typed toy programming language that can be easily embedded within c3 / c code \
it provides strong typing at compile-time in order to create robust and easy-to-scale applications blazingly fast applications !

\
neylang syntax:

```lua
u64 getu64()
  return 5
end

let i = getu64() -- type deduction
u64 b = i
```

WIP: 
  - [x] Lexer.
  - [x] Ast.
  - [x] Parser.
  - [ ] bytecode generator
  - [ ] bytecode interpreter
