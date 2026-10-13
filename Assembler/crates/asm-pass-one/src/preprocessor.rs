use asm_core::error::*;
use std::fs;
use std::collections::HashMap;

fn open_file(source_file: &str) -> Result<String, AsmError>{

    let Ok(file_content) = fs::read_to_string(source_file) else {
        return Err(AsmError::new(
            AsmErrorType::PREPRO(format!("failed to open source file {source_file}"))));
    };

   Ok(file_content) 
}

fn insert_start_end_directives(sum_files: &mut String, file_name : &str) {
    sum_files.insert_str(0, format!("@start {file_name}\n").as_str());
    sum_files.push_str(format!("\n@end {file_name}").as_str());
}


fn extract_include_path(text: &str) -> Option<&str> {
    // 1. Find where `@include` starts
    let include_part = text.split("@include").nth(1)?;

    // 2. Find the opening quote
    let start = include_part.find('"')? + 1;
    let rest = &include_part[start..];

    // 3. Find the closing quote
    let end = rest.find('"')?;

    Some(&rest[..end])
}





pub fn run(source_files: &[String], source_dir: &String) -> Result<String, Vec<AsmError>>{

    let mut errors: Vec<AsmError> = Vec::new();
    let mut file_strings_map: HashMap<&str, String> = HashMap::new();

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

        insert_start_end_directives(&mut file_string, source_file);

        file_strings_map.entry(source_file).or_insert(file_string);
    }

    let first_source_file = &source_files[0];   
    let first_file_string = match file_strings_map.get(first_source_file.as_str()) {
        Some(string) => {string},
        None => {
            errors.push(AsmError::new(AsmErrorType::PREPRO("No source files found".to_string())));
            &String::from("")
        },
    };

    let mut all_files = String::from("");
    let mut line_stack: Vec<String> = first_file_string
        .lines()
        .map(|s| s.to_string())
        .collect();
    line_stack.reverse();

    let mut included_paths: Vec<String> = Vec::new();

    while let Some(line) = line_stack.pop(){
        if line.contains("@include"){
            if let Some(path) = extract_include_path(&line){
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

        all_files.push_str((line + "\n").as_str());
    }

    println!("{all_files:?}");

    if !errors.is_empty(){
        return Err(errors);
    }

    Ok(all_files)
}