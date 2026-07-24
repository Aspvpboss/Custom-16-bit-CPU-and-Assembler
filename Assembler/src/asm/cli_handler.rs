use std::env;
use crate::error_handling::{AsmError, AsmErrorType};

pub struct Flags;

impl Flags {
    const NONE: u8 = 0;
    const ERROR: u8 = 1 << 7;
    const OPTIMIZATION: u8 = 1 << 0;
    const TOKEN_DEBUG: u8 = 1 << 1;
    const PRE_AST_DEBUG: u8 = 1 << 2;
    const POST_AST_DEBUG: u8 = 1 << 3;
}

pub enum Output_File {
    TXT(String),
    BIN(String),
}



pub struct Assembly_Commands{
    pub root_file: String,
    pub output_file: Output_File,
    pub flags: u8,

}



impl Assembly_Commands {

    pub fn print(&self){

        println!("Assembly commands:");
        println!("==================");

        println!("Root file: {}", self.root_file);
        match self.output_file {
            Output_File::BIN(ref name) => {println!("Output file: {}", name)},
            Output_File::TXT(ref name) => {println!("Output file: {}", name)},
        }

        if self.flags & Flags::ERROR != 0 {
            println!("Error flag is set")
        }
        if self.flags & Flags::OPTIMIZATION != 0 {
            println!("Optimization flag is set")
        }
        if self.flags & Flags::TOKEN_DEBUG != 0 {
            println!("Token debug flag is set")
        }
        if self.flags & Flags::PRE_AST_DEBUG != 0 {
            println!("Pre-AST debug flag is set")
        }
        if self.flags & Flags::POST_AST_DEBUG != 0 {
            println!("Post-AST debug flag is set")
        }
        
        println!("==================");
    }

}



fn is_flag(prev: &String, current: &String) -> u8 {

    if prev.as_str() == "-d" {
        match current.as_str() {

            "Tokens" => {return Flags::TOKEN_DEBUG},
            "Pre-AST" => {return Flags::PRE_AST_DEBUG},
            "Post-AST" => {return Flags::POST_AST_DEBUG},
            _ => {return Flags::ERROR},
        }
    }

    if current.as_str() == "-O" {
        return Flags::OPTIMIZATION;
    } 

    Flags::NONE
}

fn is_valid_file_ex(string: &String, valid_extensions: &[&str]) -> Option<String> {

    if let Some((_, ext)) = string.rsplit_once('.'){
        for valid_ext in valid_extensions{
            if ext == *valid_ext {
                return Some(ext.to_string());
            }
        }
    }

    None
}


fn is_valid_output(prev: &String, current: &String) -> Option<Output_File>{

    if prev != "-o"{
        return None;            
    }

    let Some(ext) = is_valid_file_ex(current, &["txt", "bin"]) else{
        return None
    };

    match ext.as_str() {
        "txt" => return Some(Output_File::TXT(current.clone())),
        "bin" => return Some(Output_File::BIN(current.clone())),
        _ => return None,
    };

}


fn is_valid_input(prev: &String, current: &String) -> Option<String>{

    if prev == "-d" || prev == "-o"{
        return None;
    }

    let Some(_) = is_valid_file_ex(current, &["txt", "asm"]) else{
        return None
    };

    Some(current.clone())
}


fn handle_errors((root_file, output_file, flags): (Option<String>, Option<Output_File>, u8), prev_arg: &String) -> Result<Assembly_Commands, Vec<AsmError>> {

    let mut errors: Vec<AsmError> = Vec::new();
    let mut should_error = false;


    let root_file = match root_file {
        Some(string) => {string},
        None => {
            errors.push(AsmError::new("No root file given".to_string(), AsmErrorType::Input));
            should_error = true;
            String::from("")
        },
    };

    let output_file = match output_file {
        Some(output) => {output},
        None => {
            errors.push(AsmError::new("No output file given".to_string(), AsmErrorType::Input));
            should_error = true;
            Output_File::BIN(String::from(""))
        }
    };


    if flags & Flags::ERROR != 0 {
        errors.push(AsmError::new("Incorrect flag given".to_string(), AsmErrorType::Input));
        should_error = true;
    }

    if prev_arg == "-o" {
        errors.push(AsmError::new("No second arg given for '-o'".to_string(), AsmErrorType::Input));
        should_error = true;
    }

    if prev_arg == "-d" {
        errors.push(AsmError::new("No second arg given for '-d'".to_string(), AsmErrorType::Input));
        should_error = true;
    }

    if should_error {
        return Err(errors);
    }

    Ok(Assembly_Commands{root_file: root_file, output_file: output_file, flags: flags})
}

pub fn get_assembly_commands() -> Result<Assembly_Commands, Vec<AsmError>>{

    let args: Vec<String> = env::args().skip(1).collect();

    let mut prev_arg: String = "".to_string();

    let mut output_file: Option<Output_File> = None;
    let mut root_file: Option<String> = None;
    let mut flags: u8 = 0;

    for arg in args {
        
        flags |= is_flag(&prev_arg, &arg);
       
        if output_file.is_none() {
            output_file = is_valid_output(&prev_arg, &arg);
        }

        if root_file.is_none(){
            root_file = is_valid_input(&prev_arg, &arg);
        } 
        
        prev_arg = arg;
    }


    match handle_errors((root_file, output_file, flags), &prev_arg){
        Ok(commands) => {return Ok(commands)},
        Err(errors) => {return Err(errors)},
    }

}
