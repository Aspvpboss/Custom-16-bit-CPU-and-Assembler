use crate::asm::cli_handler;
use crate::error_handling::AsmError;
use crate::asm::files;
use crate::asm::tokenize;

pub fn run() -> Result<(), Vec<AsmError>>{
    
    // cli_handler::get_assembly_commands()?;    

    let raw_file = files::read_and_process_file("test.txt")?;
    let tokens = tokenize::tokenize(raw_file, &mut Vec::new())?;

    for s in &tokens.raw_tokens{
        println!("F: {} L:{} C:{} ----- {} ", s.file_name, s.line, s.column, s.raw_token);
    }

    println!("");
    for s in &tokens.included_files {
        println!("included file: {s}");
    }

    Ok(())
}

