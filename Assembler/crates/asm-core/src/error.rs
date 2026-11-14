use std::fmt;
use std::rc::Rc;



pub enum AsmErrorType {
    DEBUG(String),
    CLI(String),
    PREPRO(String),
}



pub struct SrcLocation<'a> {
    pub src_file: Rc<&'a str>,
    pub row: usize,
}


pub struct AsmError<'a> {
    pub err_type : AsmErrorType,
    pub src_location : Option< SrcLocation<'a>>,
}

impl AsmError<'_> {
    
    pub fn new(error_type : AsmErrorType) -> Self {
        AsmError { err_type: error_type, src_location: None }
    }
    pub fn add_location(&mut self, location: SrcLocation) {
        self.src_location = Some(location)
    }

}


impl fmt::Display for AsmErrorType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            AsmErrorType::DEBUG(s) => write!(f, "Debug Error: {s}"), 
            AsmErrorType::CLI(s) => write!(f, "CLI Error: {s}"), 
            AsmErrorType::PREPRO(s) => write!(f, "Preprocessor Error: {s}"), 
        }
    }
}

impl fmt::Display for SrcLocation {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}:{}", self.src_file, self.row)
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

