use crate::error::*;
use std::fs;

pub fn write_dump_file(name: &str, data: String) -> Result<(), AsmError> {

    if let Err(_) = fs::create_dir_all("debug"){
        return Err(AsmError::new(AsmErrorType::DEBUG("failed to create directory 'debug'".to_string())));
    };

    let file_name = format!("debug/{}", name);

    if let Err(_) = fs::write(&file_name, data){
        return Err(AsmError::new(AsmErrorType::DEBUG(format!("failed to create {}", file_name))));
    };

    Ok(())
}