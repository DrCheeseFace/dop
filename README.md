DOP - Data Oriented Programming Lanuage
====


| Name               | Literal   |
| :---               | :---      |
| OP_MULTIPLY        | *         |
| OP_DIVIDE          | /         |
| OP_ADD             | +         |
| OP_SUBTRACT        | -         |
| OP_ASSIGN          | =         |
| OP_EQUALS          | ==        |
| OP_NEQUALS         | !=        |
| LITERAL_NUM        | 0-9       |
| LITERAL_IDENTIFIER | a-zA-Z0-9 |
| KEYWORD_RETURN     | return    |
| DELIM_OPENPAREN    | (         |
| DELIM_CLOSEPAREN   | )         |
| DELIM_OPENCURLY    | {         |
| DELIM_CLOSECURLY   | }         |
| DELIM_ENDSTATEMENT | ;         |
| TYPE_U8            | U8        |

- type info accessable
- ez debug shizz
- TODO - bump allocator

/* [typeu8, identifier(main), openbracket, closebracket, opencurly, keywordreturn, literalnum(3), closecurley] */
U8 main()
{
    return 3;
}

                        [ Program ]
                             │
                  [ FunctionDeclaration ]
                             │
    ┌───────┬────────┬───────┼───────┬────────────────┬───────────┐
    │       │        │       │       │                │           │
    ▼       ▼        ▼       ▼       ▼                ▼           ▼
 ["U8"]  ["main"]  ["("]   [")"]   ["{"]        [ Statement ]   ["}"]
                                                      │
                                       ┌──────────────┼──────────────┐
                                       │              │              │
                                       ▼              ▼              ▼
                                  ["return"]        ["3"]          [";"]


Core requirements include the following:

    Variable types must be preserved, as well as the location of each declaration in source code.
    The order of executable statements must be explicitly represented and well defined.
    Left and right components of binary operations must be stored and correctly identified.
    Identifiers and their assigned values must be stored for assignment statements.

These requirements can be used to design the data structure for the AST.

                             [ Program ]
                                  │
                                  ▼
                      [ FunctionDeclaration ]
                                  │
          ┌───────────────┬───────┴───────┬───────────────┐
          │               │               │               │
          ▼               ▼               ▼               ▼
    [ ReturnType ]     [ Name ]     [ Parameters ]     [ Body ]
         (U8)          ("main")      (Empty List)  (BlockStatement)
                                                          │
                                                          ▼
                                                  [ ReturnStatement ]
                                                          │
                                                          ▼
                                                  [ IntegerLiteral ]
                                                         (3)
