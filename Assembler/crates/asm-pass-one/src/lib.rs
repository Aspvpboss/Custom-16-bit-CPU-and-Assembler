mod preprocessor;

use asm_core::error::*;

pub fn run(source_files: &[String]) -> Result<(), Vec<AsmError>>{

    preprocessor::run(source_files)?;

    Ok(())
}
