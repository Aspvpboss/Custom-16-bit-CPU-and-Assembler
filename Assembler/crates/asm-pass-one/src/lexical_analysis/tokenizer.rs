
#[test]
fn tokenize_test() {
    println!("-------------Test---------------");

    let mut source = String::from("Yo Gurt    "); 
    println!("The source string: '{}'", source);
    
    while let Some((token, remaining)) = next_token(source) {

        println!("token '{}' : left '{}'", token, remaining);
        source = remaining;
    }

    println!("All gone");
}




pub fn next_token(source_file: String) -> Option<(String, String)>{

    if source_file.is_empty() {
        return None
    }

    let mut token_start_index: Option<usize> = None;
    let mut prev_char: char = ' ';
    let mut start_index: Option<usize> = None;
    let mut iter = source_file.char_indices();
    while let Some((index, char)) = iter.next() {
        if start_index.is_none() {
            start_index = Some(index);
        }
        if char.is_alphanumeric() && token_start_index.is_some(){
            // nothing
        } else if char.is_whitespace(){
            if let Some(start_index) = token_start_index {
                return Some((source_file[start_index..index].to_string(), source_file[index..].trim_start().to_string()))
            }

        } else if char.is_alphanumeric() && token_start_index.is_none() {
            token_start_index = Some(index);
        }
        prev_char = char;
    }

    if let Some(start_index) = start_index {
        return Some((source_file[start_index..].to_string(), "".to_string()))    
    }

    None
}

