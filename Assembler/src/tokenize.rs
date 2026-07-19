
enum State{
    Whitespace,
    Token,
    Quote
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
    Struct,
    StructType(String),
    Include,
    Label(Labels),
}

pub enum Token {
    Ident(Identifiers),
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


/*
    will tokenize and lex a processed file
*/
pub fn lexical_analysis(string: &String) -> Vec<Token> {

    let mut tokens: Vec<Token> = Vec::new();

    tokens    
}




