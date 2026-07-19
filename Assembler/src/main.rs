use std::io;
mod tokenize;
mod files;


fn main() {

    let raw_lines = files::read_and_process_file("test.txt").expect("failed to read file");

    for raw_line in raw_lines {
        println!("{}: {}", raw_line.line_number, raw_line.string);
    }

    // let mut buf = String::new();
    // io::stdin().read_line(&mut buf).expect("error typeshift");

    // let tokens = tokenize::tokenize_string(&buf);

}

