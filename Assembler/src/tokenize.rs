use crate::files::RawFileLines;



enum State{
    Whitespace,
    Character,
    Quote,
    LCurly,
    RCurly,
    LBracket,
    RBracket,
    LParen,
    RParen,
    Colon,
    Semicolon,
    Dot,
    Slash
}



pub enum Labels {
    Function(String),
    StringLit(String),
    Array(String),
}




pub enum Identifiers {
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
}


pub enum RawToken {
    Ident(Identifiers),
    Direct(Directives),
    Number(String),
    StringLit(String),
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

pub struct Token{
    token: RawToken,
    line_num: u32,
}


/*
    will tokenize and lex a processed file
*/
pub fn lexical_analysis(raw_file: RawFileLines) -> Vec<Token> {

    let mut tokens: Vec<Token> = Vec::new();

    tokens    
}




