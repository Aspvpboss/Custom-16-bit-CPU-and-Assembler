use crate::asm::first_pass_lexer::{LexedToken};
use crate::error_handling::{AsmError, AsmErrorType};
use crate::asm::symbols::*;




fn second_pass_lexer(first_pass_tokens: Vec<LexedToken>) -> Result<Vec<LexedToken>, Vec<AsmError>>{

    let mut table = SymbolTable::new();

    Ok(first_pass_tokens)
}