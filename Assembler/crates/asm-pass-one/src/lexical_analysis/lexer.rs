use std::rc::Rc;
use asm_core::error::*;


pub enum Directives {
    GLOBAL,
}

pub enum TokenType {
    DIR(Directives),
    UNKNOWN(String),
}

pub struct Token {
    r#type: TokenType,    
    location: SrcLocation,
}



pub fn lex_token(token: &str, line: usize, name: Rc<str>) -> Token {

    
    Token { r#type: TokenType::UNKNOWN(token.to_string()), location: SrcLocation { src_file: name, row: line } }
}