use std::io;
mod tokenize;



fn main() {

    let mut buf = String::new();
    io::stdin().read_line(&mut buf).expect("error typeshift");

    let tokens = tokenize::tokenize_string(&buf);


    for (i, token) in tokens.iter().enumerate() {
        println!("i: {} token: {}", i, token);
    }
}

