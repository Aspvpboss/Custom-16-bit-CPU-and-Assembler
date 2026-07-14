




# Test Syntax



#### Structs
```

struct Vec2{
    x : word,
    y : word
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




#### Intial program flow
```
// the start of the program

Second_Stage_Loader_Size: .word {sizeof(Second_Stage_Loader, Program_File)};

Second_Stage_Loader:
    // cooler program loading


Program_File: .word {ptr_one, ptr_two, ptr_three}; // contains all start labels of all programs in rom

ptr_one:


```


#### Random testing
```

// comments
// r0 - byte register
// wr0 - word register
// fr0 - float register



W_Array: .word {1, 3, 5, 7};
Array: .byte {1, 3, 5, 7};

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


player_one:
    .allocate sizeof(Player);

LOAD r2 [player_one, Player.x];



static_start:

Big_Array: .allocate .word[1024];
Empty_Array: .allocate 0;
player_one: .allocate Player;


static_end:



```