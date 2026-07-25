use crate::asm::files::{AsmFile, RawFileLines, read_and_process_file};
use crate::error_handling::{AsmError, AsmErrorType};
use std::rc::Rc;

#[derive(Debug, Clone, Copy, PartialEq)]
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

pub struct RawToken{
    pub raw_token: String,
    pub line: u32,
    pub column: u32,
    pub file_name: Rc<String>,
}

pub struct Tokens{
    pub included_files: Vec<String>,
    pub raw_tokens: Vec<RawToken>,
}


#[derive(Debug, Clone, Copy, PartialEq, Eq)]
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




fn flush_token(raw_line: &RawFileLines, tokens: &mut Vec<RawToken>, token_byte_index: &mut usize, byte_index: usize, column: u32, file_name: &Rc<String>){
    if *token_byte_index != byte_index{
        let slice = raw_line.string[*token_byte_index..byte_index].to_string();
        tokens.push(RawToken {raw_token: slice, line: raw_line.line_number, column: column, file_name: Rc::clone(file_name)});
        *token_byte_index = byte_index;
    } 
}

fn push_token(raw_line: &RawFileLines, str: &str, tokens: &mut Vec<RawToken>, column: u32, file_name: &Rc<String>){
    tokens.push(RawToken { raw_token: str.to_string(), line: raw_line.line_number, column: column, file_name: Rc::clone(file_name)});
}



fn is_included(included_files: &Vec<String>, file_name: &String) -> Option<String>{

    for included_file in included_files {
        if file_name == included_file {
            return None;
        }
    }

    Some(file_name.clone())
}


fn resolve_include(tokens: Tokens) -> Result<Tokens, Vec<AsmError>>{

    let mut errors: Vec<AsmError> = Vec::new();
    let mut raw = tokens.raw_tokens;
    let mut included_files = tokens.included_files;

    let mut i = 0;
    while i < raw.len() {

        if raw[i].raw_token != ".include" {
            i += 1;
            continue;
        }

        match (raw.get(i + 1), raw.get(i + 2), raw.get(i + 3)) {
            (Some(q1), Some(file_token), Some(q2))
                if q1.raw_token == "\"" && q2.raw_token == "\"" => {
                    
                    let Some(file_name) = is_included(&included_files, &file_token.raw_token) else{
                        i += 4;
                        continue;
                    };


                    match read_and_process_file(file_name.as_str()) {
                        Ok(raw_file) => {
                            match tokenize(raw_file, &mut included_files) {
                                Ok(included_tokens) => {

                                    raw.splice(i+4..i+4, included_tokens.raw_tokens); 

                                    for f in included_tokens.included_files {
                                        if !included_files.contains(&f) {
                                            included_files.push(f);
                                        }
                                    }

                                    i += 4;
                                },
                                Err(token_errors) => {
                                    errors.extend(token_errors);
                                    i += 4;
                                }
                            }
                        },
                        Err(file_errors) => {
                            errors.extend(file_errors);
                            errors.push(AsmError::new(
                                format!("Failed to include {}", file_name),
                                AsmErrorType::Include,
                            ).with_location(file_token.line, file_token.column, file_token.file_name.to_string()));
                            i += 4;
                        }
                    }
                }
            _ => {
                errors.push(AsmError::new(
                    "malformed .include directive, expected .include \"file_name\"".to_string(),
                    AsmErrorType::Include,
                ).with_location(raw[i].line, raw[i].column, raw[i].file_name.to_string()));
                
                i += 1;
            }
        }       

    }

    if !errors.is_empty() {
        return Err(errors);
    }

    Ok(Tokens {raw_tokens: raw, included_files: included_files})
}




pub fn tokenize(asm_file: AsmFile, included_files: &mut Vec<String>) -> Result<Tokens, Vec<AsmError>>{

    included_files.push(asm_file.name.clone());
    let file_name = Rc::new(asm_file.name.clone());
    let mut tokens: Vec<RawToken> = Vec::new();

    for raw_line in asm_file.raw_lines {

        let mut token_byte_index = 0;
        let mut current_state = State::Whitespace;
        let mut prev_state = State::Whitespace;
        let mut current_column: u32 = 0;
        let mut mode = Mode::Token;

        for (byte_index, ch) in raw_line.string.char_indices() {
            current_state = get_state(ch);
            current_column += 1;

            if mode == Mode::Token{
                // if matches!(prev_state, State::Quote){
                if prev_state == State::Quote{
                    flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column, &file_name);
                    token_byte_index = byte_index;
                    mode = Mode::InString;
                }
            } else {
                if prev_state == State::Quote{
                    mode = Mode::Token;
                }
            }

            match mode{
                Mode::Token => {
                    if current_state != State::Whitespace && prev_state == State::Whitespace {
                        token_byte_index = byte_index;
                    }

                    // Handle comment
                    if current_state == State::Slash && prev_state == State::Slash{
                        break;
                    }

                    // Handle ':' and '::'
                    if current_state == State::Colon {
                        
                        if prev_state == State::Character {
                            flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column, &file_name);
                        }

                        if prev_state == State::Colon {
                            tokens.pop();
                            token_byte_index = byte_index + 1;
                            push_token(&raw_line, "::", &mut tokens, current_column, &file_name);
                            prev_state = State::Whitespace;
                            continue;
                        }

                        // Single ':'
                        push_token(&raw_line, ":", &mut tokens, current_column, &file_name);

                        prev_state = State::Colon;
                        token_byte_index = byte_index + 1;
                        continue;
                    }

                    // handles single tokens ('{', '}', '[', ']')
                    if match_single_chars(&prev_state) {
                        flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column, &file_name);
                    } 
                    // handles full tokens and consective single tokens 
                    if (match_single_chars(&current_state) || current_state == State::Whitespace) && prev_state == State::Character{
                        flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column - 1, &file_name);
                    }
                        
                    prev_state = current_state;
                }

                Mode::InString => {
                    if current_state == State::Quote {
                        flush_token(&raw_line, &mut tokens, &mut token_byte_index, byte_index, current_column, &file_name);
                    }
                    
                    prev_state = current_state;
                }
            }
        }

        if prev_state != State::Whitespace && !(current_state == State::Slash && prev_state == State::Slash){
            let slice = raw_line.string[token_byte_index..].to_string();
            if !slice.is_empty() {
                tokens.push(RawToken {raw_token: slice, line: raw_line.line_number, column: current_column, file_name: Rc::clone(&file_name)});
            } 
        }
    }


    return resolve_include(Tokens {raw_tokens: tokens, included_files: included_files.to_vec()})
}


