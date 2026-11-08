
fn is_special_char(c: char) -> bool {
    match c {
        '{' => {
            return true;
        }
        '}' => {
            return true;
        }
        '[' => {
            return true;
        }
        ']' => {
            return true;
        }
        ';' => {
            return true;
        }
        ':' => {
            return true;
        }
        _ => {
            return false;    
        }
    }
}


fn is_normal_char(c: char) -> bool {
    return c.is_ascii_graphic() || c == '_' || c == '#'
}


pub fn next_token(source_file: String) -> Option<(String, String)>{

    if source_file.is_empty() {
        return None
    }

    let mut token_start_index: Option<usize> = None;
    let mut prev_char: char = ' ';
    let mut iter = source_file.char_indices();
    while let Some((index, char)) = iter.next() {

        if is_special_char(prev_char) && token_start_index.is_some() {
            if let Some(start_index) = token_start_index {
                return Some((source_file[start_index..index].to_string(), source_file[index..].trim_start().to_string()))
            }
        } else if is_special_char(char) && token_start_index.is_some() {
            if let Some(start_index) = token_start_index {
                return Some((source_file[start_index..index].to_string(), source_file[index..].trim_start().to_string()))
            }
        } else if is_normal_char(char) && token_start_index.is_some(){
            // nothing
        } else if char.is_whitespace(){
            if let Some(start_index) = token_start_index {
                return Some((source_file[start_index..index].to_string(), source_file[index..].trim_start().to_string()))
            }

        } else if (is_normal_char(char) || is_special_char(char)) && token_start_index.is_none() {
            token_start_index = Some(index);
        }
        prev_char = char;
    }

    if let Some(start_index) = token_start_index {
        return Some((source_file[start_index..].to_string(), "".to_string()))    
    }

    None
}

