mod preprocessor;

use asm_core::error::*;

pub fn run(source_files: &[String]) -> Result<(), Vec<AsmError>>{

    let preprocessed_string = preprocessor::run(source_files)?;
    println!("{preprocessed_string}");

    Ok(())
}
