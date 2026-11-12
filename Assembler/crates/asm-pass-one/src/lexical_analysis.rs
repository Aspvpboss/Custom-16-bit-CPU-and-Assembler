use asm_cli::user_args::{UserArgs, UserFlags};
use asm_core::error::*;
pub mod tokenizer;



/*
    Desired pipeline
    Tokenizer -> &str (for each token) 
    Lexer -> Token (contains string and line location)
    Symbol Table -> Symbol (if applicable)
*/

#[derive(Clone)]
struct TokenLocation<'a> {
    line: usize,
    name: &'a str,
}


pub fn run(mut preprocessed_src: String, user_args: &UserArgs) -> Result<(), Vec<AsmError>> {
    let mut preprocessed_src = preprocessed_src.as_str();
    let mut errors: Vec<AsmError> = Vec::new();
    let mut token_dump: Vec<String> = Vec::new();
    let is_token_dump = user_args.user_flags.contains(&UserFlags::DumpPreprocessor);
    let mut prev_token = "";
    let mut token_location_stack: Vec<TokenLocation> = Vec::new();
    let mut current_location = TokenLocation {line: 0, name: ""};

    while let Some((token, remaining)) = tokenizer::next_token(preprocessed_src) {

        current_location.line += 1;

        if prev_token == "@start" {
            token_location_stack.push(current_location.clone());
            current_location = TokenLocation { line: 0, name: token };
        }

        if prev_token == "@end" {
            if let Some(prev_location) = token_location_stack.pop() {
                current_location = prev_location;
            };
        }

        if is_token_dump {
            token_dump.push(format!("{}: {}", current_location.name, token.to_string()));
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