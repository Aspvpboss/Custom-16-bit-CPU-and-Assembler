mod lexical_analysis;
mod tokenize;
mod files;
mod cli_handler;
mod error_handling;

fn main() {

    // cli_handler::get_assembly_commands();    

    let raw_file = error_handling::handle(files::read_and_process_file("burger.txt"));

    let tokens = error_handling::handle(tokenize::tokenize(raw_file));

    for s in &tokens.raw_tokens{
        println!("F: {} L:{} C:{} ----- {} ", tokens.name, s.line, s.column, s.raw_token);
    }


}

