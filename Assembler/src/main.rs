use std::io;



fn tokenize_command_line() -> Vec<String> {

    let mut buf = String::new();
    io::stdin().read_line(&mut buf).expect("error or something");
    let buf = buf.trim().to_string();

    let mut token_start_index = 0;
    let mut prev_char = ' ';

    let mut command_line_tokens: Vec<String> = Vec::new();

    for (byte_index, char) in buf.char_indices() {
        if char == ' '  && prev_char == ' '{
            prev_char = char; 
            continue;
        }

        if char != ' ' && prev_char == ' ' {
            token_start_index = byte_index;
        }

        if char == ' ' && prev_char != ' ' {
            let slice = buf[token_start_index..byte_index].to_string();
            command_line_tokens.push(slice);
        }
        
        prev_char = char; 
    }

    if prev_char != ' ' {
        command_line_tokens.push(buf[token_start_index..].to_string());
    }
    
    command_line_tokens
}


fn main() {
    let tokens = tokenize_command_line();


    for (i, token) in tokens.iter().enumerate() {
        println!("i: {} token: {}", i, token);
    }
}

