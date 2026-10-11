use asm_core::error::*;
use std::process::ExitCode;

pub fn main() -> ExitCode{

    if let Err(errors) = run(){
        for error in errors {
            println!("{}", error);
        }

        return ExitCode::FAILURE;
    };

    return ExitCode::SUCCESS;
}


fn run() -> Result<(), Vec<AsmError>>{

    println!("foo");
    asm_cli::run()?;

    asm_pass_one::run()?;
    asm_pass_two::run()?;

    Ok(())
}