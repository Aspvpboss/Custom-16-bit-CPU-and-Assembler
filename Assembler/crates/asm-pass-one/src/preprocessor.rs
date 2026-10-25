use asm_cli::user_args::{UserArgs, UserFlags};
use asm_core::error::*;
use std::fs;
use std::collections::HashMap;


pub enum DumpPreprocessor {
    YES,
    NO
}


fn vec_to_string(string_vec: Vec<String>) -> String {
    
    let mut new_string = String::from("");
    for line in string_vec {
        new_string.push_str(line.as_str());
    }

    new_string
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


    pub fn extract_include_path(text: &str) -> Option<&str> {
        // 1. Find where `@include` starts
        let include_part = text.split("@include").nth(1)?;

        // 2. Find the opening quote
        let start = include_part.find('"')? + 1;
        let rest = &include_part[start..];

        // 3. Find the closing quote
        let end = rest.find('"')?;

        Some(&rest[..end])
    }
}


fn resolve_includes(file_strings_map: HashMap<&str, String>, source_dir: &String, errors: &mut Vec<AsmError>) -> Vec<String> {

    // includes start from this file
    let first_file_string = "main.asm"; 
    let first_file_string = match file_strings_map.get((source_dir.to_string() + first_file_string).as_str()) {
        Some(string) => {string},
        None => {
            errors.push(AsmError::new(AsmErrorType::PREPRO(format!("root file {first_file_string} not found"))));
            &String::from("")
        },
    };

    let mut combined_source: Vec<String> = Vec::new();
    let mut line_stack: Vec<String> = first_file_string
        .lines()
        .map(|s| s.to_string())
        .collect();
    line_stack.reverse();

    let mut included_paths: Vec<String> = Vec::new();

    while let Some(line) = line_stack.pop(){
        if line.contains("@include"){
            if let Some(path) = Include::extract_include_path(&line){
                let src_path = source_dir.to_string() + path;
                if included_paths.contains(&src_path){
                    continue;
                }
                
                let Some(file_contents) = file_strings_map.get(src_path.as_str()) else {
                    errors.push(AsmError::new(AsmErrorType::PREPRO(format!("Failed to include {src_path}"))));
                    continue;
                };
                let mut new_file: Vec<String> = file_contents
                    .lines()
                    .map(|s| s.to_string())
                    .collect();
                new_file.reverse();
                line_stack.append(&mut new_file);
                included_paths.push(src_path);

                continue;
            }
        }

        combined_source.push(line + "\n");
    }

    combined_source   
}

struct Comments;
impl Comments {
    pub fn find_single_comment_slice(line: &str) -> Option<&str> {

        // Find start of comment
        let start = line.find("//")?;
        let rest = &line[start..];

        // Find the end of line
        let end = rest.find('\n')?;

        Some(&rest[..end])
    }


    pub fn find_multi_comment_slice<'a>(line: &'a str, multi_line_comment: bool) -> Option<(&'a str, bool)> {
        // start of multi line comment
        if let Some(start) = line.find("/*"){
            let rest = &line[start..];
            // Find the end of line
            if let Some(end_multi_line) = rest.find("*/"){
                return Some((&rest[..end_multi_line + 2], false));
            }
            let end = rest.find('\n')?;
            return Some((&rest[..end], true));
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

fn remove_comments(combined_source: Vec<String>, _errors: &mut Vec<AsmError>) -> Vec<String>{

    let mut new_combined_source: Vec<String> = Vec::new();
    let mut multi_line_comment: bool = false;

    for line in combined_source {

        match Comments::find_single_comment_slice(&line) {
            Some(comment_portion) => {
                let comment_removed = line.replace(comment_portion, "");
                new_combined_source.push(comment_removed);
                continue;
            }
            None => {
                if let Some((comment_portion, is_multi_line)) = Comments::find_multi_comment_slice(&line, multi_line_comment){
                    let blank = Comments::mask_comment_with_spaces(comment_portion);
                    let comment_removed = line.replace(comment_portion, &blank);
                    new_combined_source.push(comment_removed);
                    multi_line_comment = is_multi_line;
                    continue;
                }
            }
        } 

        new_combined_source.push(line);
    }

    new_combined_source
}

/*
@macro macro_name macro_content
*/
struct Macro;
impl Macro{

    pub fn find_and_get_macro_name(line: &str) ->  Option<(&str, &str)>{
        let macro_part = line.split("@macro").nth(1)?;
        let macro_info: Vec<&str> = macro_part.split_whitespace().collect();

        let macro_name = *macro_info.get(0)?;
        let macro_content = *macro_info.get(1)?;

        Some((macro_name, macro_content))
    }

    pub fn check_and_expand_macro(line: &str, macro_table: &HashMap<String, String>) -> String{

        let line = line.to_string().replace(",", " ");
        let tokenized_line: Vec<&str> = line.split_whitespace().collect();
        let mut new_line = line.to_string();

        for token in tokenized_line {
            if let Some(macro_content) = macro_table.get(token){
                new_line = new_line.replace(token, macro_content);
            }
        }


        new_line
    }

}


fn expand_macros(combined_source: Vec<String>, _errors: &mut Vec<AsmError>) -> String{

    let mut macro_table: HashMap<String, String> = HashMap::new();
    let mut new_combined_source: Vec<String> = Vec::new();

    for line in combined_source {
        if let Some((macro_name, macro_content)) = Macro::find_and_get_macro_name(&line){
            macro_table.entry(macro_name.to_string()).or_insert(macro_content.to_string());
            new_combined_source.push("\n".to_string());
            continue;
        }
        let new_line = Macro::check_and_expand_macro(&line, &macro_table);
        new_combined_source.push(new_line);
    }

    println!("{:?}", macro_table);

    vec_to_string(new_combined_source)
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
    let root_file_vec = match file_vec_map.get((user_args.src_directory.to_string() + root_file).as_str()) {
        Some(string_vec) => {string_vec},
        None => {
            errors.push(AsmError::new(AsmErrorType::PREPRO(format!("root file {root_file} not found"))));
            &Vec::new()
        },
    };

    for line in root_file_vec {
        
    }

    let combined_source = resolve_includes(file_strings_map, &user_args.src_directory, &mut errors);
    let combined_source = remove_comments(combined_source, &mut errors);
    let combined_source = expand_macros(combined_source, &mut errors);

    if user_args.user_flags.contains(&UserFlags::DumpPreprocessor){
        if let Err(error) = write_dump_file(combined_source.clone()){
            errors.push(error);
        }
    }

    if !errors.is_empty(){
        return Err(errors);
    }

    Ok(combined_source)
}
