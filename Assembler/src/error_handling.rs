

pub enum AsmErrorType{
    Input,
}



pub struct AsmError{

    message: String,
    err_type: AsmErrorType,
    line: Option<u32>,
    column: Option<u32>,

}

impl AsmError{

    pub fn new(error_message: String, error_type: AsmErrorType, error_line: Option<u32>, error_column: Option<u32>) -> AsmError{
        AsmError{ message: error_message, err_type: error_type, line: error_line, column: error_column}
    }

}


impl std::fmt::Display for AsmError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let err_type_str = match self.err_type {
            AsmErrorType::Input => "Input Error",
            _ => "",
        };
        if let Some(line) = self.line {
            if let Some(column) = self.column {
                return write!(f, "At {}:{}, {} {}", line, column, err_type_str, self.message)
            }
        }

        write!(f, "{} {}", err_type_str, self.message)
    }
}



pub struct ErrorHandler{

    errors: Vec<AsmError>,
    panic: bool

}


