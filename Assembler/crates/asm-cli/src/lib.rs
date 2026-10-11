pub mod user_args;

use asm_core::error::*;
use user_args::UserArgs;
use std::env;

fn output_command(iter: &mut impl Iterator<Item = String>, output_file_name: &Option<String>) -> Result<()>{

    Ok(())
}

pub fn run() {
    let mut args = env::args().skip(1);

    let output_file_name: Option<String> = None;
    
    while let Some(arg) = args.next() {
        match arg.as_str() {
            "-o" => {

            }

            _ => {

            }
        }
    }

}