use std::rc::Rc;
use asm_core::error::*;

enum TokenType {

}

struct Token {
    r#type: TokenType,    
    location: SrcLocation,
}



pub fn lex_token(token: &str, line: usize, name: Rc<&str>) -> Token {

    
    Token { r#type: (), location: SrcLocation { src_file: name, row: line } }
}