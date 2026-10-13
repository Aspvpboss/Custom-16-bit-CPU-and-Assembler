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

    let user_args = asm_cli::run()?;
    asm_pass_one::run(&user_args.src_files, &user_args.src_directory)?;
    asm_pass_two::run()?;

    Ok(())
}