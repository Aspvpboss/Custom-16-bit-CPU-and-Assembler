mod lexical_analysis;
mod tokenize;
mod files;
mod cli_handler;
mod error_handling;
mod run;

use std::process::ExitCode;

fn main() -> ExitCode{

    match run::run() {
        Ok(()) => ExitCode::SUCCESS,
        Err(errors) => {

            for error in &errors {
                eprintln!("{error}");
            }

            ExitCode::FAILURE
        }
    }

}

