use asm_cli::user_args::{UserArgs, UserFlags};
use asm_core::error::*;
pub mod tokenizer;



/*
    Desired pipeline
    Tokenizer -> &str (for each token) 
    Lexer -> Token (contains string and line location)
    Symbol Table -> Symbol (if applicable)
*/

pub fn run(mut preprocessed_src: String, user_args: &UserArgs) {

    let mut token_dump: Vec<String> = Vec::new();
    let is_token_dump = user_args.user_flags.contains(&UserFlags::DumpPreprocessor);

    while let Some((token, remaining)) = tokenizer::next_token(preprocessed_src) {

        if is_token_dump {
            token_dump.push(token.clone());
        }

        println!("{}", token);        
        preprocessed_src = remaining;
    }

}