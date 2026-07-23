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



pub fn read_and_process_file(file_name: &str) -> Result<AsmFile, Vec<AsmError>>{

    let mut raw_lines = vec![
        RawFileLines{
            string: format!(".file_start \"{file_name}\""),
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

    for(line_num, line) in reader.lines().enumerate() {
        let Ok(line) = line else{
            let file_error = AsmError::new(
                format!("failed read line in {}", file_name),
                AsmErrorType::File);
            
            return Err(vec![file_error]);        
        };

        raw_lines.push(RawFileLines {
            string: line,
            line_number: (line_num as u32) + 1,
        });
    }

    raw_lines.push(RawFileLines {
         string: format!(".file_end \"{file_name}\""), 
         line_number: 0, 
    });


    let asm_file = AsmFile{name: file_name.to_string(), raw_lines: raw_lines};
    
    Ok(asm_file)
}