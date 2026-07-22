


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



pub struct ErrorHandler{

    errors: Vec<AsmError>,

}


impl ErrorHandler {

    pub fn new() -> ErrorHandler{
        ErrorHandler { errors: Vec::new()}
    }


    pub fn handle<T>(&mut self, result: Result<T, Vec<AsmError>>) -> T{

        let input_errors = match result {
            Ok(value) => {return value},
            Err(error) => error,
        };

        let mut should_panic = false;
        for error in &input_errors {
            if error.panic {should_panic = true};
        }

        self.errors.extend(input_errors);

        if should_panic {
            for error in &self.errors{
                println!("{}", error);
            }

            panic!();
        }

    }

}

