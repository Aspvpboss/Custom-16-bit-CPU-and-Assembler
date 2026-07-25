



pub struct FunctionLabel {
    address : u16,
}


pub struct DataLabel {
    address : u16,
}

pub struct VarType {
    size: u8,
}


pub struct Macro {

}



pub enum Symbol{
    FunctionLabel(FunctionLabel),
    DataLabel(DataLabel),
    VarType(VarType),
    Macro(Macro),
}



pub struct SymbolTable{
    

}