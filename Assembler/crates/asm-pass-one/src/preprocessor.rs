use asm_core::error::*;
use std::fs;

fn open_file(source_file: &str) -> Result<String, AsmError>{

    let Ok(file_content) = fs::read_to_string(source_file) else {
        return Err(AsmError::new(
            AsmErrorType::PREPRO(format!("failed to open source file {source_file}"))));
    };

   Ok(file_content) 
}

pub fn run(source_files: &[String]) -> Result<String, Vec<AsmError>>{

    let mut sum_files = String::from("");
    let mut errors: Vec<AsmError> = Vec::new();

    let first_source_file = &source_files[0];   
    let file_string = match open_file(first_source_file){
        Ok(file_content) => {
            Some(file_content)
        }

        Err(error) => {
            errors.push(error);
            None
        }
    };
    if let Some(file_string) = file_string {
        sum_files.push_str(&file_string);
    }

    if !errors.is_empty(){
        return Err(errors);
    }

    Ok(sum_files)
}