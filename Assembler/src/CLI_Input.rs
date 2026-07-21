use std::env;

pub struct Flags;

impl Flags {
    const NONE: u8 = 0;
    const OPTIMIZATION: u8 = 1 << 0;
    const TOKEN_DEBUG: u8 = 1 << 1;
    const PRE_AST_DEBUG: u8 = 1 << 2;
    const POST_AST_DEBUG: u8 = 1 << 3;
}

pub enum Output_File {
    NONE,
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
            _ => {return Flags::NONE},
        }
    }

    if current.as_str() == "-O" {
        return Flags::OPTIMIZATION;
    } 

    Flags::NONE
}


pub fn get_assembly_commands() -> Result<Assembly_Commands, Vec<String>>{

    let args: Vec<String> = env::args().collect();

    let mut prev_arg: String = "".to_string();
    let mut asm_commands = Assembly_Commands{root_file: String::from(""), output_file: Output_File::NONE, asm_flags: Flags::NONE};

    for arg in args {
        
        asm_commands.asm_flags |= is_flag(&prev_arg, &arg);

        prev_arg = arg;
    }


    Ok(asm_commands)
}
