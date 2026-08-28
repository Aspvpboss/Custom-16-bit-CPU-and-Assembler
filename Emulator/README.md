

# Address space


0x0000 - 0x00FF: hardcoded bootloader (256B)

0x0100 - 0x6EFF: RAM (~27.5 KiB)

0x6F00 - 0x7EFF: VRAM (4 KiB, 64x64 @ 1 byte/pixel)

0x7F00 - 0x7FFF: MMIO (256B)

0x8000 - 0xFFFF: Selected RAM bank (32 KiB for each bank)
    can support techinically up to 2 ^ 16 amount of banks,
    each bank being a binary file that is loaded

most functions will XIP (execute in place), but there should be a function in loaded by the bootloader from rom into ram that will switch banks, so that the program isn't running from the rom bank while it is switching banks

# Addressing modes


##### (regA and regB are 4-bit register addresses )
##### (regC is a 5-bit register address)

| Opcode | Mode | Op0 | Op1 | Op2 | Extra Bytes |
|:---:|:---:|:---:|:---:|:---:| ---:| 
| 000 | Reg Direct | regA | regC | regB | 2 |
| 001 | Reg Indirect | regA | regC | n/a | 2 |
| 010 | Reg imm8 | regA | regC | imm8 | 3 |
| 011 | Reg imm16 | regA | regC | imm16 | 4 |
| 100 | Immediate8-dest | imm8 | regC | n/a | 2 |
| 101 | Immediate16-dest | imm16 | reg C | n/a | 3 |
| 110 | Immediate8 | imm8| n/a | n/a | 1 |
| 111 | Immediate16 | imm16 | n/a | n/a | 2 |


# Registers
| ALU registers | FPU registers |
|:---:|:---:|
| 8 16-bit registers, stack pointer, frame pointer, and rom bank register | 8 16-bit IEEE floating point registers |

FPU and ALU operations only use 4-bit addresses for registers; however, for memory operations, an additional high bit extends this field to 5 bits, where the extra bit specifies the transfer width (0 = 8-bit word, 1 = 16-bit byte)
    

# Instruction Set - (32 instructions)


### 5 memory instructions
| LOAD | loads from memory into a register |
|:---:|:---|
| Reg Indirect | [regA] -> RegC|
| Reg imm8 | [regA + imm8] -> RegC|
| Reg imm16 | [regA + imm16] -> RegC|
| Imm8-dest | [imm8] -> RegC|
| Imm16-dest | [imm16] -> RegC|
##### You can use LOAD as a load immediate; however, you do need the immediate to be in the currently loaded bank or in ram

| STR | stores register into address in memory |
|:---:|:---|
| Reg Indirect | RegC -> [regA] |
| Reg imm8 | RegC -> [regA + imm8] |
| Reg imm16 | RegC -> [regA + imm16] |
| Imm8-dest | RegC -> [imm8] |
| Imm16-dest | RegC -> [imm16] |
##### Trying to store a register value to the ROM section will cause the emulator to throw an error and close


| PUSH | pushs a register into memory|
|:---:|:---|
| Imm8 | RegC -> [SP]|

| POP | pops a register from memory|
|:---:|:---|
| Imm8 | [SP] -> RegC|
##### PUSH and POP both use the imm8 as the register address to save memory

| MOV | inter-register movement operations |
|:---:|:---|
| Reg Direct | RegA -> RegC |


| Opcode | Mode |
| :---: | :---: |
| 0000 | Reg to reg |
| 0001 | Reg to float |
| 0010 | Float to reg |
| 0011 | Float to float |

##### This is used in the regB operand for the MOV instruction to select what it does. The full 16-bits will be moved (no 8-bit).


### 6 control flow instructions

| SYSCALL | call special functions not in memory |
|:---:|:---|
| Imm8 | a system call |
| Imm16 | a system call |

| JMP | jumps to an address |
|:---:|:---|
| Reg Indirect |  jump using regA value |
| Imm8-dest |  relative jump using imm8 |
| Imm16-dest | relative jump using imm16 |
| Imm8 | absolute jump using imm8 |
| Imm16 | absolute jump using imm16 |

| JIF | Conditional JMP |
|:---:|:---|
| Reg Indirect |  cond jump using regA value |
| Imm8-dest |  relative cond jump using imm8 |
| Imm16-dest | relative cond jump using imm16 |
##### JIF uses regC as condition selector

| CAL | jumps to a function, pushes current address into stack |
|:---:|:---|
| Reg Indirect |  calls using regA value |
| Imm8-dest |  relative call using imm8 |
| Imm16-dest | relative call using imm16 |
| Imm8 | absolute call using imm8 |
| Imm16 | absolute call using imm16 |

| CIF | Conditional CAL |
|:---:|:---|
| Reg Indirect |  cond jump using regA value |
| Imm8-dest |  relative cond jump using imm8 |
| Imm16-dest | relative cond jump using imm16 |
##### CIF uses regC as condition selector

| RET | returns from a function call |
|:---:|:---|
| imm8 | n/a |

##### Condition selector used for JIF and CIF

| Opcode| Condition flag |
|:---:|:---|
| 0000 | Equal |
| 0001 | Not equal |
| 0010 | If 0 |
| 0011 | Unsigned less than |
| 0100 | Unsigned less than or equal to |
| 0101 | Unsigned greater than |
| 0110 | Unsigned greater than or equal to |
| 0111 | Signed less than |
| 1000 | Signed less than or equal to |
| 1001 | Signed greater than |
| 1010 | Signed greater than or equal to |
| 1011 | Carry |
| 1100 | Overflow |

### 12 alu instructions
| ADD | adds two registers and saves to a register |
|:---:|:---|
| Reg Direct | regA + regB -> RegC |
| Reg imm8 | regA + imm8 -> RegC |
| Reg imm16 | regA + imm16 -> RegC |

| SUB | subtracts two registers and saves to a register |
|:---:|:---|
| Reg Direct | regA - regB -> RegC |
| Reg imm8 | regA - imm8 -> RegC |
| Reg imm16 | regA - imm16 -> RegC |

| MUL | multiplies two registers and saves to a register |
|:---:|:---|
| Reg Direct | regA * regB -> RegC |
| Reg imm8 | regA * imm8 -> RegC |
| Reg imm16 | regA * imm16 -> RegC |

| DIV | divides two registers and saves to a register |
|:---:|:---|
| Reg Direct | regA / regB -> RegC |
| Reg imm8 | regA / imm8 -> RegC |
| Reg imm16 | regA / imm16 -> RegC |

| MOD | modulos two registers and saves to a register |
|:---:|:---|
| Reg Direct | regA % regB -> RegC |
| Reg imm8 | regA % imm8 -> RegC |
| Reg imm16 | regA % imm16 -> RegC |

| CMP | compares two registers and updates the flags |
|:---:|:---|
| Reg Direct | regA ? regB -> flags |
| Reg imm8 | regA ? imm8 -> flags |
| Reg imm16 | regA ? imm16 -> flags|

| AND | ands two registers and saves to a register |
|:---:|:---|
| Reg Direct | regA & regB -> RegC |
| Reg imm8 | regA & imm8 -> RegC |
| Reg imm16 | regA & imm16 -> RegC |

| NOR | nors two registers and saves to a register |
|:---:|:---|
| Reg Direct | regA ~\| regB -> RegC |
| Reg imm8 | regA ~\| imm8 -> RegC |
| Reg imm16 | regA ~\| imm16 -> RegC |

| XOR | xors two registers and saves to a register |
|:---:|:---|
| Reg Direct | regA ^ regB -> RegC |
| Reg imm8 | regA ^ imm8 -> RegC |
| Reg imm16 | regA ^ imm16 -> RegC |

| ARS | arthimatic right shifts a register and saves to a register |
|:---:|:---|
| Reg Direct | regA >>> regB -> RegC |
| Reg imm8 | regA >>> imm8 -> RegC |
| Reg imm16 | regA >>> imm16 -> RegC |

| LRS | logical right shifts a register and saves to a register |
|:---:|:---|
| Reg Direct | regA >> regB -> RegC |
| Reg imm8 | regA >> imm8 -> RegC |
| Reg imm16 | regA >> imm16 -> RegC |

| LLS | logical left shifts a register and saves to a register |
|:---:|:---|
| Reg Direct | regA << regB -> RegC |
| Reg imm8 | regA << imm8 -> RegC |
| Reg imm16 | regA << imm16 -> RegC |

### 8 float instructions
| FADD | adds two float registers and saves to a float register |
|:---:|:---|
| Reg Direct | regA + regB -> RegC |
| Reg imm16 | regA + imm16 -> RegC |

| FSUB | subtracts two float registers and saves to a float register |
|:---:|:---|
| Reg Direct | regA - regB -> RegC |
| Reg imm16 | regA - imm16 -> RegC |

| FMUL | multiplies two float registers and saves to a float register |
|:---:|:---|
| Reg Direct | regA * regB -> RegC |
| Reg imm16 | regA * imm16 -> RegC |

| FDIV| divides two float registers and saves to a float register |
|:---:|:---|
| Reg Direct | regA / regB -> RegC |
| Reg imm16 | regA / imm16 -> RegC |

| FSQRT | square roots a float register and saves to a float register |
|:---:|:---|
| Reg Direct | sqrt(regA) -> RegC |
| Imm16-dest | sqrt(Imm16) -> RegC |

| FCMP | compares two float registers and updates the flags |
|:---:|:---|
| Reg Direct | regA ? regB -> flags |
| Reg imm16 | regA ? imm16 -> flags|

| FINT | converts a float to an integer |
|:---:|:---|
| Reg Direct | fint(regA) -> regC |

| IFLO | converts integer to float |
|:---:|:---|
| Reg Direct | iflo(regA) -> regC |




# Memory Mapped IO

### The Screen
- Emulator thread & Render Thread

- Emulator frame buffer -(memcpy)> back buffer
- Emulator frame ready -(atomic int)> swap buffers for render thread

- This means no thread stalling for rendering