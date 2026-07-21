use crate::files::RawFileLines;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
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

pub struct RawTokens{
    pub raw_token: String,
    pub line: u32,
    pub column: u32,
}


enum Mode{
    Token,
    InString,
}


fn get_state(ch: char) -> State {

    if ch.is_whitespace(){
        return State::Whitespace;
    }
    if ch.is_alphanumeric() || ch == '_' || ch == '-'{
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



// doesn't match State::Dot
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
        State::Slash => true,
        _ => false
    }
}




fn flush_token(raw_line: &RawFileLines, tokens: &mut Vec<RawTokens>, token_byte_index: &mut usize, byte_index: usize, column: u32){
    if *token_byte_index != byte_index{
        let slice = raw_line.string[*token_byte_index..byte_index].to_string();
        tokens.push(RawTokens {raw_token: slice, line: raw_line.line_number, column: column});
        *token_byte_index = byte_index;
    } 
}

fn push_token(raw_line: &RawFileLines, str: &str, tokens: &mut Vec<RawTokens>, column: u32){
    tokens.push(RawTokens { raw_token: str.to_string(), line: raw_line.line_number, column: column});
}




pub fn tokenize(raw_file: Vec<RawFileLines>) -> Vec<RawTokens>{
    let mut tokens: Vec<RawTokens> = Vec::new();

    for raw_line in raw_file {

        let mut token_byte_index = 0;
        let mut current_state = State::Whitespace;
        let mut prev_state = State::Whitespace;
        let mut current_column: u32 = 0;
        let mut mode = Mode::Token;

        for (byte_index, ch) in raw_line.string.char_indices() {
            current_state = get_state(ch);
            current_column += 1;

            if matches!(mode, Mode::Token){
                if matches!(prev_state, State::Quote){
                    flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column);
                    token_byte_index = byte_index;
                    mode = Mode::InString;
                }
            } else {
                if matches!(prev_state, State::Quote){
                    mode = Mode::Token;
                }
            }

            match mode{
                Mode::Token => {
                    if !matches!(current_state, State::Whitespace) && matches!(prev_state, State::Whitespace) {
                        token_byte_index = byte_index;
                    }

                    // Handle comment
                    if matches!(current_state, State::Slash) && matches!(prev_state, State::Slash){
                        break;
                    }

                    // Handle ':' and '::'
                    if matches!(current_state, State::Colon) {
                        
                        if matches!(prev_state, State::Character) {
                            flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column);
                        }

                        if matches!(prev_state, State::Colon) {
                            tokens.pop();
                            token_byte_index = byte_index + 1;
                            push_token(&raw_line, "::", &mut tokens, current_column);
                            prev_state = State::Whitespace;
                            continue;
                        }

                        // Single ':'
                        push_token(&raw_line, ":", &mut tokens, current_column);

                        prev_state = State::Colon;
                        token_byte_index = byte_index + 1;
                        continue;
                    }

                    // handles single tokens ('{', '}', '[', ']')
                    if match_single_chars(&prev_state) {
                        flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column);
                    } 
                    // handles full tokens and consective single tokens 
                    if (match_single_chars(&current_state) || matches!(current_state, State::Whitespace)) && matches!(prev_state, State::Character){
                        flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column - 1);
                    }
                        
                    prev_state = current_state;
                }

                Mode::InString => {
                    if matches!(current_state, State::Quote) {
                        flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column);
                    }
                    
                    prev_state = current_state;
                }
            }
        }

        if !matches!(prev_state, State::Whitespace) && !(matches!(current_state, State::Slash) && matches!(prev_state, State::Slash)){
            let slice = raw_line.string[token_byte_index..].to_string();
            if !slice.is_empty() {
                tokens.push(RawTokens {raw_token: slice, line: raw_line.line_number, column: current_column});
            } 
        }
    }

    tokens
}


