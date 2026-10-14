pub struct UserArgs{
    pub src_files: Vec<String>,
    pub user_flags: Vec<UserFlags>,
    pub output_file: String,
    pub src_directory: String,
}

#[derive(PartialEq)]
pub enum UserFlags {
    DumpPreprocessor,
}
