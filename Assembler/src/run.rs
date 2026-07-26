use crate::error_handling::AsmError;
use crate::asm::lexical_analysis;

pub fn run() -> Result<(), Vec<AsmError>>{
    
    
    let lexed_tokens = lexical_analysis::lexical_analysis()?;


    for lexed_token in &lexed_tokens{
        println!("{}", lexed_token);
    }

    Ok(())
}

