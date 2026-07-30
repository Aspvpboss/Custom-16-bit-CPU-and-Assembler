use crate::asm::first_pass_lexer::{LexedToken};
use crate::error_handling::{AsmError, AsmErrorType};


fn second_pass_lexer(first_pass_tokens: Vec<LexedToken>) -> Result<Vec<LexedToken>, Vec<AsmError>>{
    Ok(first_pass_tokens)
}