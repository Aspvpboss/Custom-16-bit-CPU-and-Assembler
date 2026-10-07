use crate::asm::first_pass_lexer::{self, LexedToken, RawLexedToken};
use crate::error_handling::{AsmError, AsmErrorType};


pub struct Macro {
    parameters: Vec<String>,
    macro_body: Vec<LexedToken>,
}



fn add_error(message: String, token: &LexedToken) -> AsmError {
    AsmError::new(message, AsmErrorType::Macro)
        .with_location(token.line, token.column, token.file_name.to_string())
}

impl Macro {
    fn parse(macro_start_slice: &[LexedToken]) -> Result<Macro, Vec<AsmError>>{
        let mut errors: Vec<AsmError> = Vec::new();
        // let RawLexedToken::Unknown(name) = macro_start_slice[1].clone() else {

        // }

        let second_token = &macro_start_slice[1];

        let macro_name = match &second_token.token {
            RawLexedToken::Unknown(string) => {string;},
            _ => {
                String::from("");
                errors.push(add_error("Invalid name for macro".to_string(), second_token))
            },    
        };

        Err(errors)
    }
}