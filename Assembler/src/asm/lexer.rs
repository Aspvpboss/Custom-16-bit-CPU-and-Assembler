
use crate::error_handling::{AsmError, AsmErrorType};
use std::rc::Rc;
use crate::asm::tokenize::Tokens;

pub enum LabelKind{
    Address,
    StringLit,
    Array,
}

pub struct Labels {
    kind: LabelKind,
    name: String,
    from_macro: bool,
}


pub enum Instruction {
    // Memory
    Load = 0,
    Str,
    Push,
    Pop,
    Mov,

    // Control Flow
    Syscall,
    Jmp,
    Jif,
    Cal,
    Cif,
    Ret,

    // Integer ALU
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Cmp,
    And,
    Nor,
    Xor,
    Ars,
    Lrs,
    Lls,

    // Floating Point
    FAdd,
    FSub,
    FMul,
    FDiv,
    FSqrt,
    FCmp,
    FInt,
    IFlo,
}


pub enum Identifiers {
    Instruction,
    Reg(String),
    WideReg(String),
    FloatReg(String),
    Label(Labels),
}

pub enum Directives {
    Include,
    Allocate,
    Struct,
    VariableType(String),
    Glob,
    Macro,
}


pub enum RawLexedToken {
    Ident(Identifiers),
    Direct(Directives),
    Number(String),
    StringLit(String),
    Unknown(String),
    FileStart,
    FileEnd,
    EOF,
    LCurly,
    RCurly,
    LBracket,
    RBracket,
    LParen,
    RParen,
    Colon,
    Semicolon,
    Dot
}

pub struct LexedToken{
    token: RawLexedToken,
    line_num: u32,
    pub line: u32,
    pub column: u32,
    pub file_name: Rc<String>,
}


/*
    will tokenize and lex a processed file
*/
pub fn lexer(tokens: Tokens) -> Result<Vec<LexedToken>, Vec<AsmError>> {

    let mut lexed_tokens: Vec<LexedToken> = Vec::new();

    Ok(lexed_tokens)
}



