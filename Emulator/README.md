

# Address space


0x0000 - 0x00FF: hardcoded bootloader (256B)

0x0100 - 0x6EFF: RAM (~27.5 KiB)

0x6F00 - 0x7EFF: VRAM (4 KiB, 64x64 @ 1 byte/pixel)

0x7F00 - 0x7FFF: MMIO (256B)

0x8000 - 0xFFFF: Selected ROM bank (32 KiB for each bank)
    can support techinically up to 2 ^ 16 amount of banks,
    each bank being a binary file that is loaded

most functions will XIP (execute in place), but there should be a function in loaded by the bootloader from rom into ram that will switch banks, so that the program isn't running from the rom bank while it is switching banks

# Addressing modes


##### (regA and regB are 4-bit register addresses )
##### (regC is a 5-bit register address)

| Opcode | Mode | Op0 | Op1 | Op2 |
|:---:|:---:|:---:|:---:|:---:|
| 000 | Reg Direct | regA | regC | regB |
| 001 | Reg Indirect | regA | regC | n/a |
| 010 | Reg imm8 | regA | regC | imm8 |
| 011 | Reg imm16 | regA | regC | imm16 |
| 100 | Immediate8-dest | imm8 | regC | n/a |
| 101 | Immediate16-dest | imm16 | reg C | n/a |
| 110 | Immediate8 | imm8| n/a | n/a |
| 111 | Immediate16 | imm16 | n/a | n/a |


# Registers
| ALU registers | FPU registers |
|:---:|:---:|
| 8 16-bit registers, stack pointer, frame pointer, and rom bank register | 8 16-bit IEEE floating point registers |

FPU and ALU operations only use 4-bit addresses for registers; however, for memory operations, an additional high bit extends this field to 5 bits, where the extra bit specifies the transfer width (0 = 8-bit word, 1 = 16-bit byte)
    

# Instruction Set

## 32 total instructions


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


#### 6 control flow instructions
- SYSCALL - system call special functions (not in memory)
- JMP - absolute or relative, depends on addressing mode
- JIF - same as jmp
- CAL - functional absolute or relative
- CIF - same as cal
- RET - returns from function call

#### 12 alu instructions
- ADD - adds two integers
- SUB - subtracts two integers
- MUL - multiplys two integers
- DIV - divides two integers
- MOD - modulos two integers
- CMP - compares two integers
- AND - ands two integers
- NOR - nors two integers
- XOR - xors two integers
- ARS - arthimatic right shifts a integer
- LRS - logical right shifts a integer
- LLS - logical left shifts a integer

#### 8 float instructions
- FADD - adds two floats
- FSUB - subtracts two floats
- FMUL - multiplys two floats
- FDIV - divides two floats
- FSQRT - takes square root of float
- FCMP - compares two floats
- FINT - converts float to integer
- IFLO - converts integer to float




# Memory Mapped IO

### The Screen
- Emulator thread & Render Thread

- Emulator frame buffer -(memcpy)> back buffer
- Emulator frame ready -(atomic int)> swap buffers for render thread

- This means no thread stalling for rendering