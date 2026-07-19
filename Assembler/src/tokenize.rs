
enum State{
    Whitespace,
    Token,
    Quote
}

pub enum Token {
    Ident(String),
    Number(String),
    StringLit(String),
    LCurly,
    RCurly,
    LBracket,
    RBracket,
    LParen,
    RParen,
    Colon,
    Semicolon,
    Dot
}

pub fn tokenize_string(string: &String) -> Vec<Token> {

    let mut tokens: Vec<Token> = Vec::new();

    tokens    
}





// pub fn tokenize_string(string: &String) -> Vec<String> {

//     let mut token_start_index = 0;
//     let mut prev_is_whitespace = true;

//     let mut command_line_tokens: Vec<String> = Vec::new();

//     for (byte_index, ch ) in string.char_indices() {

//         let is_whitespace = ch.is_whitespace();

//         if is_whitespace && prev_is_whitespace {
//             prev_is_whitespace = is_whitespace;
//             continue;
//         }

//         if !is_whitespace && prev_is_whitespace {
//             token_start_index = byte_index;
//         }

//         if is_whitespace && !prev_is_whitespace {
//             let slice = string[token_start_index..byte_index].to_string();
//             command_line_tokens.push(slice);            
//         }
        
//         prev_is_whitespace = is_whitespace; 
//     }

//     if !prev_is_whitespace {
//         command_line_tokens.push(string[token_start_index..].to_string());
//     }
    
//     command_line_tokens
// }

