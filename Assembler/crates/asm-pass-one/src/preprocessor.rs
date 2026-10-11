use asm_core::error::*;
use std::fs;

fn open_file(source_file: &str) -> Result<String, AsmError>{

    let Ok(file_content) = fs::read_to_string(source_file) else {
        return Err(AsmError::new(
            AsmErrorType::PREPRO(format!("failed to open source file {source_file}"))));
    };

   Ok(file_content) 
}

pub fn run(source_files: &[&str]) -> Result<String, AsmError>{

    let mut sum_files = String::from("");


    Ok(sum_files)
}