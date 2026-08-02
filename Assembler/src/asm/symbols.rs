use crate::asm::macros::Macro;
use crate::error_handling::{AsmError, AsmErrorType};
use std::collections::HashMap;
use std::rc::Rc;


#[derive(Default)]
pub struct FunctionLabel {
    address : u16,
    is_public: bool,
}

#[derive(Default)]
pub struct DataLabel {
    address : u16,
    is_public: bool,
}

#[derive(Default)]
pub struct VarType {
    size: u8,
    is_public: bool,
}



pub enum SymbolType{
    FunctionLabel(FunctionLabel),
    DataLabel(DataLabel),
    VarType(VarType),
    Macro(Macro),
}


pub struct Symbol {
    symbol: SymbolType,
    name : String,
}



pub struct SymbolTable{
    symbol_table : HashMap<String, Vec<Rc<Symbol>>>,
}

impl SymbolTable {
    fn new() -> SymbolTable {
        SymbolTable { symbol_table: HashMap::new() }
    }

    fn add_symbol(self, name : String, symbol_type : SymbolType) -> Result<Self, AsmError> {
        if self.symbol_table.contains_key(&name){
            return Err(AsmError::new(format!("Tried to included symbols that already exist in "), AsmErrorType::Include))
        }
        
        let symbol = Symbol {symbol: symbol_type, name: name};

        Ok(self)        
    }
}


