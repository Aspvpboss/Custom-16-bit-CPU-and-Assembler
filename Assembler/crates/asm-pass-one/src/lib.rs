mod preprocessor;

use asm_core::error::*;
use asm_cli::user_args::*;

pub fn run(user_args: &UserArgs) -> Result<(), Vec<AsmError>>{

    let preprocessed_string = preprocessor::run(user_args)?;
    println!("{preprocessed_string}");

    Ok(())
}
