
use crate::error_handling::{AsmError, AsmErrorType};
use std::rc::Rc;
use crate::asm::tokenize::{Tokens, RawToken};

#[derive(Debug, Clone, PartialEq)]
pub enum LabelKind{
    Address,
    StringLit,
    Array,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Labels {
    kind: LabelKind,
    name: String,
    from_macro: bool,
}


#[derive(Debug, Clone, PartialEq)]
pub enum Instructions {
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


#[derive(Debug, Clone, PartialEq)]
pub enum Identifiers {
    Instruction(Instructions),
    Reg(u8),
    WideReg(u8),
    FloatReg(u8),
    Label(Labels),
}

#[derive(Debug, Clone, PartialEq)]
pub enum Directives {
    Include,
    Allocate,
    Struct,
    VariableType(String),
    Glob,
    Macro,
}


#[derive(Debug, Clone, PartialEq)]
pub enum NumberTypes {
    Int(i32),
    Float(f32),
}


#[derive(Debug, Clone, PartialEq)]
pub enum RawLexedToken {
    Ident(Identifiers),
    Direct(Directives),
    Number(NumberTypes),
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
    DoubleColon,
    Semicolon,
}

pub struct LexedToken{
    token: RawLexedToken,
    pub line: u32,
    pub column: u32,
    pub file_name: Rc<String>,
}

impl LexedToken{

    pub fn new(lexed_token: RawLexedToken, og_token: RawToken) -> LexedToken{
        LexedToken { 
            token: lexed_token, 
            line: og_token.line, 
            column: og_token.column, 
            file_name: Rc::clone(&og_token.file_name) 
        }
    }

}



/*
    will tokenize and lex a processed file
*/
pub fn lexer(tokens: Tokens) -> Result<Vec<LexedToken>, Vec<AsmError>> {

    let mut lexed_tokens: Vec<LexedToken> = Vec::new();

    Ok(lexed_tokens)
}



