use asm_cli::user_args::{UserArgs, UserFlags};
use asm_core::error::*;
pub mod tokenizer;



/*
    Desired pipeline
    Tokenizer -> &str (for each token) 
    Lexer -> Token (contains string and line location)
    Symbol Table -> Symbol (if applicable)
*/

pub fn run(mut preprocessed_src: String) {

    while let Some((token, remaining)) = tokenizer::next_token(preprocessed_src) {

        println!("{}", token);        
        preprocessed_src = remaining;
    }

}