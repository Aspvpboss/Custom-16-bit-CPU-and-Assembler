mod preprocessor;

use asm_core::error::*;

pub fn run(source_files: &[String], source_dir: &String) -> Result<(), Vec<AsmError>>{

    let preprocessed_string = preprocessor::run(source_files, source_dir)?;
    println!("{preprocessed_string}");

    Ok(())
}
