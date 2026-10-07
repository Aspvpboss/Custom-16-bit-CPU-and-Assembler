use std::fmt;




pub enum AsmErrorType {

}

pub struct AsmErrorLocation {
    pub row: u32,
    pub col: u32,
}


pub struct AsmError {
    pub err_type : AsmErrorType,
    pub src_location : Option<AsmErrorLocation>,
}

impl AsmError {
    
    fn new(error_type : AsmErrorType) -> Self {
        AsmError { err_type: error_type, src_location: None }
    }
    fn add_location(&mut self, row: u32, col : u32) {
        self.src_location = Some(AsmErrorLocation { row, col })
    }

}



impl fmt::Display for AsmErrorType {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "")
    }
}

impl fmt::Display for AsmError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "")
    }
}


type Result<T> = std::result::Result<T, AsmError>;