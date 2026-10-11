# Basic Syntax (phase 1)

## Preprocessor

The preprocessor will do basic expensions of macros and include directives.
It will also add in @start file_name @end file_name

### Directives
```
// public 
@include file_name
@macro macro_name macro_value

// non public
@start file_name
@end file_name

```

## Lexical Analysis

### Directives
```
immediates
    #10
    #0xff
    #0b0000

array type
    .byte
    .word
    .ascii

.at address
.at #0x8000

```

### Identifiers

```

// comments
/* 
    multi
    line
    comment
*/

// label
label:
    // content
    mov r0, r2;
    mov r0 [r2];
    mov r0 [r2, r0];
    add r0, r1, r2;

label: .byte {#255, #255, #-127, #4}
label: .word {#65536, #1, #2, #4}
label: .ascii "string"

```
