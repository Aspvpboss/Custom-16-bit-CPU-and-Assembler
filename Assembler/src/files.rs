use std::fs::File;
use std::io::{self, BufRead, BufReader};


pub struct RawFileLines{
    pub string : String,
    pub line_number : u32,
}



pub struct AsmFile{
    pub name: String,
    pub raw_lines: Vec<RawFileLines>,
}



pub fn read_and_process_file(file_name: &str) -> Result<AsmFile, io::Error>{

    let mut raw_lines = vec![
        RawFileLines{
            string: format!(".file_start \"{file_name}\""),
            line_number: 0
        },
    ];


    let file = File::open(file_name)?;
    let reader = BufReader::new(file);

    for(line_num, line) in reader.lines().enumerate() {
        let line = line?;
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