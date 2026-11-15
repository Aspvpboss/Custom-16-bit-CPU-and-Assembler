use std::{rc::Rc, thread::current};
use asm_cli::user_args::{UserArgs, UserFlags};
use asm_core::error::*;
pub mod tokenizer;
pub mod lexer;


/*
    Desired pipeline
    Tokenizer -> &str (for each token) 
    Lexer -> Token (contains string and line location)
    Symbol Table -> Symbol (if applicable)
*/

// #[derive(Clone)]
// struct TokenLocation {
//     line: usize,
//     name: Rc<str>,
// }


pub fn run(mut preprocessed_src: String, user_args: &UserArgs) -> Result<(), Vec<AsmError>> {

    let is_token_dump = user_args.user_flags.contains(&UserFlags::DumpPreprocessor);
    let mut preprocessed_src = preprocessed_src.as_str();
    let mut errors: Vec<AsmError> = Vec::new();
    let mut token_dump: Vec<String> = Vec::new();
    let mut prev_raw_token = "";
    let mut token_location_stack: Vec<SrcLocation> = Vec::new();
    let mut current_location = SrcLocation {src_file: Rc::from(""), row: 0};

    while let Some((raw_token, remaining)) = tokenizer::next_token(preprocessed_src, &mut current_location.row) {

        if raw_token == "@start" || raw_token == "@end" {
            prev_raw_token = raw_token;
            preprocessed_src = remaining;
            continue;
        }

        if prev_raw_token == "@start" {
            token_location_stack.push(current_location.clone());
            current_location = SrcLocation {src_file: Rc::from(raw_token), row: 0};
            prev_raw_token = raw_token;
            preprocessed_src = remaining;
            continue;
        }

        if prev_raw_token == "@end" {
            if let Some(prev_location) = token_location_stack.pop() {
                current_location = prev_location;
            };
            prev_raw_token = raw_token;
            preprocessed_src = remaining;
            continue;
        }

        if is_token_dump {
            token_dump.push(format!("{}: {}: {}", current_location.src_file, current_location.row, raw_token.to_string()));
        }

        let token = lexer::lex_token(raw_token, current_location.row, Rc::clone(&current_location.src_file));

        prev_raw_token = raw_token;
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