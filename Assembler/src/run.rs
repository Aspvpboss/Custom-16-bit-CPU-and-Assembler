use crate::error_handling::AsmError;

pub fn run() -> Result<(), Vec<AsmError>>{
    



    for s in &tokens.raw_tokens{
        println!("F: {} L:{} C:{} ----- {} ", s.file_name, s.line, s.column, s.raw_token);
    }

    println!("");
    for s in &tokens.included_files {
        println!("included file: {s}");
    }

    Ok(())
}

