use crate::error_handling::{AsmError, AsmErrorType};
use crate::asm::files;
use crate::asm::tokenize;
use crate::asm::cli_handler;
use crate::asm::first_pass_lexer::{self, LexedToken};



pub fn lexical_analysis() -> Result<Vec<LexedToken> ,Vec<AsmError>>{

    let commands = cli_handler::get_assembly_commands()?;    

    let raw_root_file = files::read_and_process_file(&commands.root_file)?;
    let (lexed_tokens, _) = tokenize::tokenize(raw_root_file, &mut Vec::new())?;

    Ok(lexed_tokens)
}


