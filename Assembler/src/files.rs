use std::fs::File;
use std::io::{self, BufRead, BufReader};


pub struct RawFileLines{
    pub string : String,
    pub line_number : u32,
}




pub fn read_and_process_file(file_name: &str) -> Result<Vec<RawFileLines>, io::Error>{

    let mut raw_lines: Vec<RawFileLines> = Vec::new();

    let file = File::open(file_name)?;
    let reader = BufReader::new(file);

    for(line_num, line) in reader.lines().enumerate() {
        let line = line?;
        raw_lines.push(RawFileLines {
            string: line,
            line_number: (line_num as u32) + 1,
        });
    }

    Ok(raw_lines)
}