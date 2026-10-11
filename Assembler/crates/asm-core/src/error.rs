use std::fmt;




pub enum AsmErrorType {
    CLI(String),
    PREPRO(String),
}

pub struct AsmErrorLocation {
    pub src_file: String,
    pub row: u32,
    pub col: u32,
}


pub struct AsmError {
    pub err_type : AsmErrorType,
    pub src_location : Option<AsmErrorLocation>,
}

impl AsmError {
    
    pub fn new(error_type : AsmErrorType) -> Self {
        AsmError { err_type: error_type, src_location: None }
    }
    pub fn add_location(&mut self, src_file: String, row: u32, col : u32) {
        self.src_location = Some(AsmErrorLocation { src_file, row, col })
    }

}


impl fmt::Display for AsmErrorType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            AsmErrorType::CLI(s) => write!(f, "CLI Error: {s}"), 
            AsmErrorType::PREPRO(s) => write!(f, "Preprocessor Error: {s}"), 
        }
    }
}

impl fmt::Display for AsmErrorLocation {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}:{}:{}", self.src_file, self.row, self.col)
    }
}


impl fmt::Display for AsmError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        if let Some(src_location) = &self.src_location {
            write!(f, "{}: {}", src_location, self.err_type)
        } else {
            write!(f, "{}", self.err_type)
        }
    }
}

