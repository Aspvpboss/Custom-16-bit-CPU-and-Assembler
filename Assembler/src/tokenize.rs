use std::thread::current;

use crate::files::RawFileLines;

enum State{
    Whitespace,
    Character,
    Quote,
    LCurly,
    RCurly,
    LBracket,
    RBracket,
    LParen,
    RParen,
    Colon,
    Semicolon,
    Dot,
    Slash
}


fn get_state(ch: char) -> State {

    if ch.is_whitespace(){
        return State::Whitespace;
    }
    if ch.is_alphanumeric() || ch == '_'{
        return State::Character;
    }

    match ch {
        '"' => State::Quote,
        '{' => State::LCurly,
        '}' => State::RCurly,
        '[' => State::LBracket,
        ']' => State::RBracket,
        '(' => State::LParen,
        ')' => State::RParen,
        ':' => State::Colon,
        ';' => State::Semicolon,
        '.' => State::Dot,
        '/' => State::Slash,
        _ => State::Whitespace
    }
}


fn match_single_chars(state: &State) -> bool {
    match state { 
        State::Quote => true,
        State::LCurly => true,
        State::RCurly => true,
        State::LBracket => true,
        State::RBracket => true,
        State::LParen => true,
        State::RParen => true,
        State::Colon => true,
        State::Semicolon => true,
        State::Dot => true,
        State::Slash => true,
        _ => false
    }
}




pub fn tokenize(raw_file: Vec<RawFileLines>) -> Vec<String>{
    let mut tokens: Vec<String> = Vec::new();

    for raw_line in raw_file {

        let mut token_byte_index = 0;
        let mut prev_state = State::Whitespace;

        let mut in_string = false;

        for (byte_index, ch) in raw_line.string.char_indices() {
            let current_state = get_state(ch);

            if !matches!(current_state, State::Whitespace) && matches!(prev_state, State::Whitespace) {
                token_byte_index = byte_index;
            }

            if matches!(current_state, State::Quote) {
                in_string = !in_string;
            }

            if in_string == false && matches!(prev_state, State::Quote){

            }

            if match_single_chars(&prev_state) {
                let slice = raw_line.string[token_byte_index..byte_index].to_string();
                tokens.push(slice);
                token_byte_index = byte_index;
            } 
            
            if (match_single_chars(&current_state) || matches!(current_state, State::Whitespace)) && matches!(prev_state, State::Character){
                let slice = raw_line.string[token_byte_index..byte_index].to_string();
                tokens.push(slice);
                token_byte_index = byte_index;
            }



            prev_state = current_state;
        }

        if match_single_chars(&prev_state) {
            let slice = raw_line.string[token_byte_index..].to_string();
            tokens.push(slice);
        }
        
    }



    tokens
}