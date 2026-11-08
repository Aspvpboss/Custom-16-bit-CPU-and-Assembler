use asm_cli::user_args::{UserArgs, UserFlags};
use asm_core::error::*;
pub mod tokenizer;



/*
    Desired pipeline
    Tokenizer -> &str (for each token) 
    Lexer -> Token (contains string and line location)
    Symbol Table -> Symbol (if applicable)
*/

pub fn run(mut preprocessed_src: String, user_args: &UserArgs) -> Result<(), Vec<AsmError>> {

    let mut errors: Vec<AsmError> = Vec::new();
    let mut token_dump: Vec<String> = Vec::new();
    let is_token_dump = user_args.user_flags.contains(&UserFlags::DumpPreprocessor);

    while let Some((token, remaining)) = tokenizer::next_token(preprocessed_src) {

        if is_token_dump {
            token_dump.push(token.clone());
        }

        preprocessed_src = remaining;
    }

    if is_token_dump {
        if let Err(error) = asm_core::debug::write_dump_file("TOKEN_DUMP.txt", token_dump.join("\n")){
            errors.push(error);
        };
    }

    if !errors.is_empty() {
        return Err(errors);
    }

    Ok(())
}