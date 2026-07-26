
use std::rc::Rc;
use std::collections::HashMap;
use crate::error_handling::{AsmError, AsmErrorType};
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
    pub token: RawLexedToken,
    pub line: u32,
    pub column: u32,
    pub file_name: Rc<String>,
}

impl LexedToken{

    pub fn new(lexed_token: RawLexedToken, og_token: &RawToken) -> LexedToken{
        LexedToken { 
            token: lexed_token, 
            line: og_token.line, 
            column: og_token.column, 
            file_name: Rc::clone(&og_token.file_name) 
        }
    }

}

impl std::fmt::Display for LexedToken {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {

        write!(
            f,
            "{}:{}:{} - {:?}",
            self.file_name, self.line, self.column, self.token
        )
    }
}


fn lex_instruction_token(token: &RawToken) -> Option<RawLexedToken> {
    match token.raw_token.as_str() {

        _ => None
    } 
}


fn lex_simple_tokens(token: &RawToken) -> LexedToken {

    let lex_type = match token.raw_token.as_str() {

        ".file_start" => {Some(RawLexedToken::FileStart)},
        ".file_end" => {Some(RawLexedToken::FileEnd)},


        _ => {None}    
    };



    match lex_type{
        Some(lex_type) => LexedToken::new(lex_type, token),
        None => LexedToken::new(RawLexedToken::Unknown(token.raw_token.clone()), token)
    } 
}


/*
    will tokenize and lex a processed file
*/
pub fn lexer(tokens: Tokens) -> Result<Vec<LexedToken>, Vec<AsmError>> {

    let mut lexed_tokens: Vec<LexedToken> = Vec::new();

    let mut tokens = tokens.raw_tokens;

    let mut i = 0;
    while i < tokens.len() {
        let token = &tokens[i];

        lexed_tokens.push(lex_simple_tokens(token));

        i += 1;
    } 

    Ok(lexed_tokens)
}



