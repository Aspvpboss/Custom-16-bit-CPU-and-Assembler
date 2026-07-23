


struct AsmResult<T>(Result<T, Vec<AsmError>>);




pub enum AsmErrorType{
    Input,
}


struct ErrorLocation{
    line: u32,
    column: u32,
    file_name: String,
}


pub struct AsmError{

    message: String,
    err_type: AsmErrorType,
    location: Option<ErrorLocation>,
    panic: bool,

}

impl AsmError{

    pub fn new(error_message: String, error_type: AsmErrorType) -> AsmError{
        AsmError{ message: error_message, err_type: error_type, location: None, panic: false}
    }

    pub fn with_location(mut self, line: u32, column: u32, file_name: String) -> Self{
        self.location = Some(ErrorLocation{line: line, column: column, file_name: file_name});
        self
    }

    pub fn panic(mut self) -> Self{
        self.panic = true;
        self
    }

}


impl std::fmt::Display for AsmError {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let err_type_str = match self.err_type {
            AsmErrorType::Input => "Input Error",
        };

        if let Some(location) = &self.location {
            return write!(f, "{}({}, {}): {} - {}", location.file_name, location.line, location.column, err_type_str, self.message);
        }

        write!(f, "{} - {}", err_type_str, self.message)
    }
}




impl<T> AsmResult<T> {

    pub fn asm_handle(result: Result<T, Vec<AsmError>>) -> T{

        match result {
            Ok(success) => success,
            Err(errors) => {

                for error in &errors {
                    println!("{}", error);
                } 
                panic!();
            }
        }
        
    }

}

