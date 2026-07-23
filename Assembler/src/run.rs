use crate::cli_handler;
use crate::error_handling::AsmError;
use crate::files;
use crate::tokenize;

pub fn run() -> Result<(), Vec<AsmError>>{
    cli_handler::get_assembly_commands();    

    let raw_file = files::read_and_process_file("burger.txt")?;
    let tokens = tokenize::tokenize(raw_file)?;

    for s in &tokens.raw_tokens{
        println!("F: {} L:{} C:{} ----- {} ", tokens.name, s.line, s.column, s.raw_token);
    }

    Ok(())
}

