use asm_cli::user_args::{UserArgs, UserFlags};
use asm_core::error::*;
use std::fs;
use std::collections::HashMap;


pub enum DumpPreprocessor {
    YES,
    NO
}



fn open_file(source_file: &str) -> Result<String, AsmError>{

    let Ok(file_content) = fs::read_to_string(source_file) else {
        return Err(AsmError::new(
            AsmErrorType::PREPRO(format!("failed to open source file {source_file}"))));
    };

   Ok(file_content) 
}



fn source_file_to_string_hashmap<'a>(source_files: &'a [String], errors: &mut Vec<AsmError>) -> HashMap<&'a str, Vec<String>>{
    let mut file_strings_map: HashMap<&str, Vec<String>> = HashMap::new();

    for source_file in source_files {
        let mut file_string = match open_file(source_file){
            Ok(file_content) => {
                file_content
            }

            Err(error) => {
                errors.push(error);
                continue;
            }
        };
        
        Include::insert_start_end_directives(&mut file_string, source_file);
        let mut line_stack: Vec<String> = file_string
            .lines()
            .map(|s| s.to_string())
            .collect();
        line_stack.reverse();


        file_strings_map.entry(source_file).or_insert(line_stack);
    }

    file_strings_map
}

struct Include;
impl Include {
    pub fn insert_start_end_directives(sum_files: &mut String, file_name : &str) {
        sum_files.insert_str(0, format!("@start {file_name}\n").as_str());
        sum_files.push_str(format!("\n@end {file_name}").as_str());
    }


    pub fn extract_include_path<'a>(text: &'a str, errors: &mut Vec<AsmError>) -> Option<&'a str> {
        // 1. Find where `@include` starts
        let include_part = text.split("@include").nth(1)?;

        // 2. Find the opening quote
        let Some(mut start) = include_part.find('"') else{
            errors.push(AsmError::new(AsmErrorType::PREPRO(format!("@include directive doesn't have proper closing quotes"))));
            return None
        };
        start += 1;
        let rest = &include_part[start..];

        // 3. Find the closing quote
        let Some(end) = rest.find('"') else {
            errors.push(AsmError::new(AsmErrorType::PREPRO(format!("@include directive doesn't have proper closing quotes"))));
            return None;
        };

        Some(&rest[..end])
    }
}


fn resolve_includes(
    file_vec_map: &HashMap<&str, Vec<String>>, line: &str, 
    included_paths: &mut Vec<String>, source_dir: &str, errors: &mut Vec<AsmError>) -> Option<Vec<String>>{

    let mut new_lines: Vec<String> = Vec::new();

    if !line.contains("@include") {
        return None;
    }

    if let Some(path) = Include::extract_include_path(&line, errors){
        let src_path = source_dir.to_string() + path;
        if included_paths.contains(&src_path){
            return Some(vec!["".to_string()]);
        }
        
        let Some(file_contents) = file_vec_map.get(src_path.as_str()) else {
            errors.push(AsmError::new(AsmErrorType::PREPRO(format!("Failed to include {src_path}"))));
            return None                    
        };
        new_lines.append(&mut file_contents.clone());
        included_paths.push(src_path);

    }

    if new_lines.is_empty() {
        return None;
    }

    Some(new_lines)
}

struct Comments;
impl Comments {
    pub fn find_single_comment_slice(line: &str) -> Option<&str> {

        // Find start of comment
        let start = line.find("//")?;

        Some(&line[start..])
    }


    pub fn find_multi_comment_slice<'a>(line: &'a str, multi_line_comment: bool) -> Option<(&'a str, bool)> {
        // start of multi line comment
        if let Some(start) = line.find("/*"){
            let rest = &line[start..];
            // Find the end of line
            if let Some(end_multi_line) = rest.find("*/"){
                return Some((&rest[..end_multi_line + 2], false));
            }
            return Some((rest, true));
        }

        // end of multi line comment
        if let Some(end_multi_line) = line.find("*/"){
            return Some((&line[..end_multi_line + 2], false));        
        }
        
        // if in multi line block, keep going
        if multi_line_comment {
            return Some((line, true));
        }

        None
    }

    pub fn mask_comment_with_spaces(comment: &str) -> String {
        comment
            .chars()
            .map(|c| if c == '\n' { '\n' } else { ' ' })
            .collect()
    }
        
}

fn remove_comments(mut line: String, multi_line_comment: &mut bool, _errors: &mut Vec<AsmError>) -> String {
    if let Some((comment_portion, is_multi_line)) = Comments::find_multi_comment_slice(&line, *multi_line_comment){
        let blank = Comments::mask_comment_with_spaces(comment_portion);
        line = line.replace(comment_portion, &blank);
        *multi_line_comment = is_multi_line;
    } else {
        if let Some(comment_portion) = Comments::find_single_comment_slice(&line) {
            let blank = Comments::mask_comment_with_spaces(comment_portion);
            line = line.replace(comment_portion, &blank);
        }
    }

    line
}

/*
@macro macro_name macro_content
*/
struct Macro;
impl Macro{

    pub fn find_and_get_macro_name(line: &str, errors: &mut Vec<AsmError>) ->  Option<(String, String, String)>{
        let mut original_line = line.to_string();
        let macro_part = line.split("@macro").nth(1)?;
        let macro_info: Vec<&str> = macro_part.split_whitespace().collect();

        let Some(&macro_name) = macro_info.get(0) else {
            errors.push(AsmError::new(AsmErrorType::PREPRO("Invalid @macro definition, no macro name given".to_string())));
            return None;
        };
        let Some(&macro_content) = macro_info.get(1) else {
            errors.push(AsmError::new(AsmErrorType::PREPRO("Invalid @macro definition, no macro content given".to_string())));
            return None;
        };

        let start = original_line.find("@macro")?;
        let end = original_line.find(macro_content)? + macro_content.len();

        original_line.replace_range(start..end, " ");

        Some((original_line, macro_name.to_owned(), macro_content.to_owned()))
    }

    pub fn check_and_expand_macro(line: &str, macro_table: &HashMap<String, String>) -> String{

        let line = line.to_string().replace(",", " ");
        let tokenized_line: Vec<&str> = line.split_whitespace().collect();
        let cleaned_tokens: Vec<String> = tokenized_line
            .iter()
            .map(|token| token.replace("[", "").replace("]", ""))
            .collect();
        let mut new_line = line.to_string();
        let mut search_pos = 0;

        for token in cleaned_tokens {
            if let Some(rel_idx) = new_line[search_pos..].find(&token) {
                let start = search_pos + rel_idx;
                let end = start + token.len();
                if let Some(macro_content) = macro_table.get(&token) {
                    new_line.replace_range(start..end, macro_content);
                    search_pos = start + macro_content.len();
                } else {
                    search_pos = end;
                }
            }
        }

        new_line
    }

}

// expands macros AND replaces commas w/ spaces
fn expand_macros(mut line: String, macro_table: &mut HashMap<String, String>, errors: &mut Vec<AsmError>) -> String{

    if let Some((new_line, macro_name, macro_content)) = Macro::find_and_get_macro_name(&line, errors) {
        macro_table.entry(macro_name.to_string()).or_insert(macro_content.to_string());
        line = new_line;
    }


    let new_line = Macro::check_and_expand_macro(&line, &macro_table);

    new_line
}


fn write_dump_file(preprocessor_data: String) -> Result<(), AsmError> {

    if let Err(_) = fs::write("PREPROCESSOR_DUMP.txt", preprocessor_data){
        return Err(AsmError::new(AsmErrorType::PREPRO("failed to open PREPROCESSOR_DUMP.txt".to_string())));
    };

    Ok(())
}



pub fn run(user_args: &UserArgs) -> Result<String, Vec<AsmError>>{

    let mut errors: Vec<AsmError> = Vec::new();
    /*
        I want it to go from hashmap, to file individual strings w/ comments -> single string resolved includes -> macros 
    */

    let file_vec_map = source_file_to_string_hashmap(&user_args.src_files, &mut errors);

    let root_file = "main.asm"; 
    let mut root_file_vec = match file_vec_map.get((user_args.src_directory.to_string() + root_file).as_str()) {
        Some(string_vec) => {string_vec.clone()},
        None => {
            errors.push(AsmError::new(AsmErrorType::PREPRO(format!("root file {root_file} not found"))));
            Vec::new()
        },
    };

    let mut combined_source_vec: Vec<String> = Vec::new(); 
    let mut included_paths: Vec<String> = Vec::new();
    let mut multi_line_comment = false;
    let mut macro_table: HashMap<String, String> = HashMap::new();

    while let Some(line) = root_file_vec.pop() {

        let line = remove_comments(line, &mut multi_line_comment, &mut errors);
        let line = expand_macros(line, &mut macro_table, &mut errors);

        if let Some(mut included_file) = resolve_includes(&file_vec_map, &line, &mut included_paths, &user_args.src_directory, &mut errors){
            root_file_vec.append(&mut included_file);
            continue;
        };


        combined_source_vec.push(line + "\n");
    }

    if multi_line_comment == true {
        errors.push(AsmError::new(AsmErrorType::PREPRO(format!("opened ended multi-line comment"))));
    }

    // root_file_vec
    let combined_source = combined_source_vec.join("");
    
    if !errors.is_empty(){
        return Err(errors);
    }

    if user_args.user_flags.contains(&UserFlags::DumpPreprocessor){
        if let Err(error) = write_dump_file(combined_source.clone()){
            errors.push(error);
        }
    }

    println!("{macro_table:?}");

    Ok(combined_source)
}
