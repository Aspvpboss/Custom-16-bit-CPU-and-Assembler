use crate::asm::cli_handler;
use crate::error_handling::AsmError;
use crate::asm::files;
use crate::asm::tokenize;

pub fn run() -> Result<(), Vec<AsmError>>{
    
    let commands = cli_handler::get_assembly_commands()?;    


    let raw_root_file = files::read_and_process_file(&commands.root_file)?;
    let tokens = tokenize::tokenize(raw_root_file, &mut Vec::new())?;

    for s in &tokens.raw_tokens{
        println!("F: {} L:{} C:{} ----- {} ", s.file_name, s.line, s.column, s.raw_token);
    }

    println!("");
    for s in &tokens.included_files {
        println!("included file: {s}");
    }

    Ok(())
}

