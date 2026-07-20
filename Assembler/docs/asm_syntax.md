




# Test Syntax



#### Structs
```

struct Vec2{
    x : .word,
    y : .word
};

struct Player{
    health : float,
    alive : byte,
    pos : Vec2
};

```

#### Addressing modes
```
LOAD r2 [base, index] // index
LOAD wr2 [base] // indirect
LOAD r0 0x7E00 // direct


```

#### Macros
```

.macro MacroName (para1, para2) {

    ADD para1 para2 #10;
    STR para2 para1;

}


```



#### Random testing
```

// comments
// r0 - byte register
// wr0 - word register
// fr0 - float register



W_Array: .word {1, 3, 5, 7};
Array: .byte {1, 3, 5, 7};

Small_Array: .allocate .byte[1024];
Big_Array: .allocate .word[1024];
player_one: .allocate Player;


String: .string "Burger\n";

Label:
ADD r0 r2 r3;
LOAD r2 0x0100;
STR [r0, r2] r2;
MOV fr0, wr2;

struct Vec2{
    x : word,
    y : word
};

struct Player{
    health : float,
    alive : byte,
    pos : Vec2
};



LOAD r2 [player_one, Player.pos.x];

```



