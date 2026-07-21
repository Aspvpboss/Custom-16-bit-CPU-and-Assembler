mod lexical_analysis;
mod tokenize;
mod files;
mod CLI_Input;

fn main() {

    CLI_Input::get_assembly_commands();    

    let raw_file = files::read_and_process_file("test.txt").expect("failed to read file");

    let mut tokens = tokenize::tokenize(raw_file);

    for s in &tokens{
        // println!("{} ", s.raw_token);
    }

    // let mut buf = String::new();
    // io::stdin().read_line(&mut buf).expect("error typeshift");

    // let tokens = tokenize::tokenize_string(&buf);

}

