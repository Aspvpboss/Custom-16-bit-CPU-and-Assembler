

pub enum AsmErrorType{
    Input
}



pub struct AsmError{

    message: String,
    err_type: AsmErrorType,

}