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

impl Symbol {
    fn new(name : String, symbol_type : SymbolType) -> Rc<Symbol> {
        Rc::new(Symbol {symbol: symbol_type, name: name.clone()})
    }
}


pub struct SymbolTable{
    symbol_table : HashMap<String, Vec<Rc<Symbol>>>,
}

impl SymbolTable {
    fn new() -> SymbolTable {
        SymbolTable { symbol_table: HashMap::new() }
    }

    fn add_symbol(&mut self, file_name : String, symbol : &Rc<Symbol>) -> Option<AsmError> {
        if let Some(existing_symbols) = self.symbol_table.get(&file_name) {
            for existing in existing_symbols {
                if existing.name == symbol.name {
                    return Some(AsmError::new(
                        format!("Duplicate symbol '{}' found in {}", existing.name, file_name),
                        AsmErrorType::Include));
                }
            }
        }

        let entry = self.symbol_table.entry(file_name).or_default();
        entry.push(Rc::clone(symbol));

        None
    }

    fn include_file_symbols(&mut self, source_file: &String, dest_file: String) -> Option<Vec<AsmError>>{

        let mut errors: Vec<AsmError> = Vec::new();
        
        let Some(source_table) = self.symbol_table.get(source_file) else{
            errors.push(AsmError::new(
                format!("Tried to included symbols from {} that doesn't exist", source_file), 
                AsmErrorType::Include));
            return Some(errors)
        };

        let symbols = source_table.clone();
        
        for symbol in &symbols {
            if let Some(error) = self.add_symbol(dest_file.clone(), symbol){
                errors.push(error);
            }
        }

        if errors.is_empty() {
            None
        } else {
            Some(errors)
        }
    }
}


