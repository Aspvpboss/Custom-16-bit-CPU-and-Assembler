pub mod preprocessor;
pub mod lexical_analysis;

use asm_core::error::*;
use asm_cli::user_args::*;

use crate::lexical_analysis::tokenizer;

pub fn run(user_args: &UserArgs) -> Result<(), Vec<AsmError>>{

    let preprocessed_string = preprocessor::run(user_args)?;

    Ok(())
}
