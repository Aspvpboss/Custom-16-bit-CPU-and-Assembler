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
    let mut preprocessed_src = preprocessed_src.as_str();
    let mut errors: Vec<AsmError> = Vec::new();
    let mut token_dump: Vec<String> = Vec::new();
    let is_token_dump = user_args.user_flags.contains(&UserFlags::DumpPreprocessor);
    let mut prev_token = "";
    let mut active_file = "";
    let mut current_token_line: usize = 0;

    while let Some((token, remaining)) = tokenizer::next_token(preprocessed_src) {

        current_token_line += 1;
        println!("line {}: file {}", current_token_line, active_file);

        if prev_token == "@start" {
            active_file = token;
            current_token_line = 0;
        }

        if is_token_dump {
            token_dump.push(token.to_string());
        }

        prev_token = token;
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