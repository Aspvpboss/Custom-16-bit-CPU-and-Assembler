use crate::asm::first_pass_lexer::{self, LexedToken};
use crate::error_handling::{AsmError, AsmErrorType};


pub struct Macro {
    macro_name: String,
    parameters: Vec<String>,
    macro_body: Vec<LexedToken>,
}



impl Macro {
    // fn parse(macro_start_slice: &[LexedToken]) -> Result<Macro, Vec<AsmError>>{
     
    // }
}