pub mod user_args;

use asm_core::error::*;
use user_args::UserArgs;
use std::env;

fn sub_command(args: &mut impl Iterator<Item = String>, optional_string: &mut Option<String>) -> Result<(), AsmError>{

    if let Some(string) = optional_string{
        return Err(AsmError::new(AsmErrorType::CLI(format!("duplicate arg {string}"))));
    }

    let Some(next_arg) = args.next() else{
        return Err(AsmError::new(AsmErrorType::CLI(format!("opened ended sub-command"))));
    }; 

    *optional_string = Some(next_arg);

    Ok(())
}

fn valid_file_extension(arg: &String, file_extensions : &Vec<&str>) -> Result<(), AsmError> {

    // splits into substrings, skip file name
    let v: Vec<&str> = arg.as_str().split_terminator('.').skip(1).collect();

    let Some(file_extension) = v.get(0) else {
        return Err(AsmError::new(AsmErrorType::CLI(format!("failed to get file_extension for file {arg}"))));
    };

    if !file_extensions.contains(file_extension){
        return Err(AsmError::new(AsmErrorType::CLI(
            format!("invalid file extension {file_extension} for file {arg}. Valid file extensions are {file_extensions:?}"))));
    }

    Ok(())
}

pub fn run() -> Result<UserArgs, Vec<AsmError>>{

    let mut errors: Vec<AsmError> = Vec::new();
    let mut args = env::args().skip(1);
    let mut output_file_name: Option<String> = None;
    let mut source_directory: Option<String> = None;
    let mut src_files: Vec<String> = Vec::new();

    let valid_source_files = vec!["asm"];
    let valid_output_files = vec!["bin"];

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
                if let Err(error) = valid_file_extension(&arg, &valid_source_files){
                    errors.push(error);
                    continue;
                }
                src_files.push(arg);
            }
        }
    }

    if src_files.is_empty() {
        errors.push(AsmError::new(AsmErrorType::CLI(String::from("No source files given"))));
    }

    let output_file_name = match output_file_name {
        Some(s) => {
            if let Err(error) = valid_file_extension(&s, &valid_output_files){
                errors.push(error);
            }
            s
        },
        None => {
            errors.push(AsmError::new(AsmErrorType::CLI(String::from("No output file given"))));
            String::from("")
        },
    };

    if !errors.is_empty() {
        return Err(errors);
    }

    Ok(UserArgs { src_files, output_file : output_file_name })
}