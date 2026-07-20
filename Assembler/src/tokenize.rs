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

enum Mode{
    Token,
    InString,
}

pub struct RawTokens{
    pub raw_token: String,
    pub line_num: u32,
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




fn flush_token(raw_line: &RawFileLines, tokens: &mut Vec<RawTokens>, token_byte_index: &mut usize, byte_index: usize){
    if *token_byte_index != byte_index{
        let slice = raw_line.string[*token_byte_index..byte_index].to_string();
        tokens.push(RawTokens {raw_token: slice, line_num: raw_line.line_number});
        *token_byte_index = byte_index;
    } 
}

fn push_token(raw_line: &RawFileLines, str: &str, tokens: &mut Vec<RawTokens>){
    tokens.push(RawTokens { raw_token: str.to_string(), line_num: raw_line.line_number});
}




pub fn tokenize(raw_file: Vec<RawFileLines>) -> Vec<RawTokens>{
    let mut tokens: Vec<RawTokens> = Vec::new();

    for raw_line in raw_file {

        let mut token_byte_index = 0;
        let mut prev_state = State::Whitespace;

        let mut chars = raw_line.string.char_indices().peekable();
        while let Some((byte_index, ch)) = chars.next() {
            let current_state = get_state(ch);

            if !matches!(current_state, State::Whitespace) && matches!(prev_state, State::Whitespace) {
                token_byte_index = byte_index;
            }
            
            // Handle ':' and '::'
            if matches!(current_state, State::Colon) {
                
                if matches!(prev_state, State::Character) {
                    flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index);
                }

                if let Some((_, next_ch)) = chars.peek() {
                    if matches!(get_state(*next_ch), State::Colon) {
                        push_token(&raw_line, "::", &mut tokens);

                        // Consume the second ':'
                        chars.next();

                        prev_state = State::Colon;
                        token_byte_index = byte_index + 2;
                        continue;
                    }
                }

                // Single ':'
                push_token(&raw_line, ":", &mut tokens);

                prev_state = State::Colon;
                token_byte_index = byte_index + 1;
                continue;
            }

            // handles single tokens ('{', '}', '[', ']')
            if match_single_chars(&prev_state) {
                flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index);
            } 
            // handles full tokens and consective single tokens 
            if (match_single_chars(&current_state) || matches!(current_state, State::Whitespace)) && matches!(prev_state, State::Character){
                flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index);
            }
                
                prev_state = current_state;
            }

            if !matches!(prev_state, State::Whitespace) {
                let slice = raw_line.string[token_byte_index..].to_string();
                tokens.push(RawTokens {raw_token: slice, line_num: raw_line.line_number});
            }
        }

    tokens
}


