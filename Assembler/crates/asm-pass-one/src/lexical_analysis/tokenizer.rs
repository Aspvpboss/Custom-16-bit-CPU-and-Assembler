
pub fn next_token(source_file: String) -> Option<(String, String)>{
    
    let mut token_start_index: Option<usize> = None;
    let mut prev_char: char = ' ';
    let mut iter = source_file.char_indices();
    
    while let Some((index, char)) = iter.next() {
        if char.is_ascii_alphanumeric() && token_start_index.is_some(){
            prev_char = char;
            continue;
        }


    }

    None
}