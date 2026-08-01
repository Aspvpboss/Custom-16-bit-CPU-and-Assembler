use std::fs::File;
use std::io::{BufRead, BufReader};
use crate::error_handling::{AsmError, AsmErrorType};


pub struct RawFileLines{
    pub string : String,
    pub line_number : u32,
}



pub struct AsmFile{
    pub name: String,
    pub raw_lines: Vec<RawFileLines>,
}

static mut FILE_ID: u32 = 0;

fn generate_file_id() -> u32 {
    unsafe{
        let current = FILE_ID;
        FILE_ID += 1; 
        current
    }
}


fn file_start_guard(line: String, line_num: usize, file_name: &str) -> Result<String, Vec<AsmError>>{

    let mut errors: Vec<AsmError> = Vec::new();

    if line.contains(".file_start"){
        errors.push(AsmError::new(
            format!("Use of .file_start in program, please remove immediately"),
            AsmErrorType::File,
        ).with_location(line_num as u32, 0, file_name.to_string()))
    }

    if line.contains(".file_end"){
        errors.push(AsmError::new(
            format!("Use of .file_end in program, please remove immediately"),
            AsmErrorType::File,
        ).with_location(line_num as u32, 0, file_name.to_string()))
    }

    if !errors.is_empty(){
        return Err(errors);
    }

    Ok(line)
}


pub fn read_and_process_file(file_name: &str) -> Result<AsmFile, Vec<AsmError>>{

    let current_id = generate_file_id();

    let mut raw_lines = vec![
        RawFileLines{
            string: format!(".file_start #{}", current_id),
            line_number: 0
        },
    ];


    let Ok(file) = File::open(file_name) else{
        let file_error = AsmError::new(
            format!("failed to open {}", file_name),
            AsmErrorType::File);
        
        return Err(vec![file_error]);        
    };


    let reader = BufReader::new(file);
    let mut errors: Vec<AsmError> = Vec::new();
    

    for(line_num, line) in reader.lines().enumerate() {
        let Ok(line) = line else{
            let file_error = AsmError::new(
                format!("failed read line in {}", file_name),
                AsmErrorType::File);
            
            return Err(vec![file_error]);        
        };

        match file_start_guard(line, line_num + 1, file_name){
            Ok(line) => {
                raw_lines.push(RawFileLines {
                    string: line,
                    line_number: (line_num as u32) + 1,
                });
            },
            Err(file_errors) => {
                errors.extend(file_errors)
            },
        }        

    }

    raw_lines.push(RawFileLines {
         string: format!(".file_end #{}", current_id), 
         line_number: 0, 
    });

    if !errors.is_empty(){
        return Err(errors)
    }

    let asm_file = AsmFile{name: file_name.to_string(), raw_lines: raw_lines};
    
    Ok(asm_file)
}