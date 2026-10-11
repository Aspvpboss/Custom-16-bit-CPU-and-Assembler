pub mod user_args;

use asm_core::error::*;
use user_args::UserArgs;
use std::env;

fn sub_command(args: &mut impl Iterator<Item = String>, optional_string: &mut Option<String>) -> Result<(), AsmError>{

    if let Some(string) = optional_string{
        return Err(AsmError::new(AsmErrorType::CLI(format!("duplicate arg {string}"))));
    }

    let Some(next_arg) = args.next() else{
        return Ok(());
    }; 

    *optional_string = Some(next_arg);

    Ok(())
}

fn valid_source_file(arg: &String) -> Result<(), AsmError> {

    Ok(())
}

pub fn run() -> Result<(), Vec<AsmError>>{

    let mut errors: Vec<AsmError> = Vec::new();
    let mut args = env::args().skip(1);
    let mut output_file_name: Option<String> = None;
    let mut source_directory: Option<String> = None;
    let mut src_files: Vec<String> = Vec::new();

    while let Some(arg) = args.next() {
        match arg.as_str() {
            "-o" => {
                if let Err(error) = sub_command(&mut args, &mut output_file_name){
                    errors.push(error);
                };
            }
            "-s" => {
                if let Err(error) = sub_command(&mut args, &mut source_directory){
                    errors.push(error);
                };
            }

            _ => { // if none else, assume source file
                if let Err(error) = valid_source_file(&arg){
                    errors.push(error);
                    continue;
                }
                src_files.push(arg);
            }
        }
    }

    if !errors.is_empty() {
        return Err(errors);
    }

    Ok(())
}