use crate::asm::files::RawFileLines;



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





pub enum Identifiers {
    Reg(String),
    WideReg(String),
    FloatReg(String),
    Label(Labels),
    MacroLabel(Labels),
}

pub enum Directives {
    Include,
    Allocate,
    Struct,
    VariableType(String),
    Glob,
    Macro,
}


pub enum LexedToken {
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

pub struct Token{
    token: LexedToken,
    line_num: u32,
}


/*
    will tokenize and lex a processed file
*/
pub fn lexical_analysis(raw_file: RawFileLines) -> Vec<Token> {

    let mut tokens: Vec<Token> = Vec::new();

    tokens    
}




