use std::rc::Rc;
use asm_core::error::*;


enum Directives {
    GLOBAL,
}

enum TokenType {
    DIR(Directives),
}

struct Token {
    r#type: TokenType,    
    location: SrcLocation,
}



pub fn lex_token(token: &str, line: usize, name: Rc<str>) -> Token {

    
    Token { r#type: TokenType::DIR(Directives::GLOBAL), location: SrcLocation { src_file: name, row: line } }
}