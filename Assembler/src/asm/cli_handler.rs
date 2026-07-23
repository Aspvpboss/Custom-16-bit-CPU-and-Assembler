use std::env;

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
    ERROR,
    TXT(String),
    BIN(String),
}




pub struct Assembly_Commands {
    pub root_file: String,
    pub output_file: Output_File,
    pub asm_flags: u8,
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

fn is_valid_output(prev: &String, current: &String) -> Output_File {
    if prev.as_str() != "-o" || current.is_empty() {
        return Output_File::ERROR;
    }

    if let Some((_, ext)) = current.rsplit_once('.') {
        match ext {
            "txt" => {return Output_File::TXT(current.clone())},
            "bin" => {return Output_File::BIN(current.clone())},
            _ => {return Output_File::ERROR},
        }
    }

    Output_File::ERROR
}



pub fn get_assembly_commands() -> Result<Assembly_Commands, Vec<String>>{

    let args: Vec<String> = env::args().skip(1).collect();

    let mut prev_arg: String = "".to_string();
    let mut asm_commands = Assembly_Commands{root_file: String::from(""), output_file: Output_File::ERROR, asm_flags: Flags::NONE};

    for arg in args {
        
        asm_commands.asm_flags |= is_flag(&prev_arg, &arg);
        asm_commands.output_file = is_valid_output(&prev_arg, &arg);

        prev_arg = arg;
    }

    match asm_commands.output_file {
        Output_File::BIN(ref string) => {println!("bin output: {}", string)},
        Output_File::TXT(ref string) => {println!("txt output: {}", string)},
        _ => {},
    }

    Ok(asm_commands)
}
