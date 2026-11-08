
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


pub fn next_token<'a>(source_file: &'a str) -> Option<(&'a str, &'a str)>{

    if source_file.is_empty() {
        return None
    }

    let mut token_start_index: Option<usize> = None;
    let mut prev_char: char = ' ';
    let mut in_string = false;
    let mut iter = source_file.char_indices();
    while let Some((index, char)) = iter.next() {

        if in_string {
            if char == '\"' {
                if let Some(start_index) = token_start_index {
                    let new_index = index + 1;
                    return Some((&source_file[start_index..new_index], source_file[new_index..].trim_start()))
                }
            }
            prev_char = char;
            continue;
        }

        if is_special_char(prev_char) && token_start_index.is_some() { // handles grabbing single characters
            if let Some(start_index) = token_start_index {
                return Some((&source_file[start_index..index], source_file[index..].trim_start()))
            }
        } else if is_special_char(char) && token_start_index.is_some() { // handles grabbing token if single character is present
            if let Some(start_index) = token_start_index {
                return Some((&source_file[start_index..index], source_file[index..].trim_start()))
            }
        } else if is_normal_char(char) && token_start_index.is_some(){ // iterates if in token
            // nothing
        } else if char.is_whitespace(){ // handles grabbing token if whitespace
            if let Some(start_index) = token_start_index {
                return Some((&source_file[start_index..index], source_file[index..].trim_start()))
            }
        } else if char == '\"' && token_start_index.is_none() { // handles start of strings
            token_start_index = Some(index);
            in_string = true;
            
        } else if (is_normal_char(char) || is_special_char(char)) && token_start_index.is_none() { // sets index for everything but strings
            token_start_index = Some(index);
        } 
        prev_char = char;
    }

    if in_string { // error branch, will add error handling soon
        return None
    }
  
    if let Some(start_index) = token_start_index {
        return Some((&source_file[start_index..], ""))    
    }

    None
}

