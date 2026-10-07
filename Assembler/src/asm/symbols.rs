use crate::asm::macros::Macro;
use crate::error_handling::{AsmError, AsmErrorType};
use std::collections::HashMap;
use std::rc::Rc;


#[derive(Default)]
pub struct FunctionLabel {
    address : u16,
}

#[derive(Default)]
pub struct DataLabel {
    address : u16,
}

#[derive(Default)]
pub struct VarType {
    size: u8,
}



pub enum SymbolType{
    FunctionLabel(FunctionLabel),
    DataLabel(DataLabel),
    VarType(VarType),
    Macro(Macro),
}


pub struct Symbol {
    pub symbol: SymbolType,
    pub name : String,
    pub is_public: bool,
}

impl Symbol {
    pub fn new(name : String, symbol_type : SymbolType, is_public : bool) -> Rc<Symbol> {
        Rc::new(Symbol {symbol: symbol_type, name: name.clone(), is_public: is_public})
    }
}


pub struct SymbolTable{
    symbol_table : HashMap<String, Vec<Rc<Symbol>>>,
}

impl SymbolTable {

    fn insert_builtin_types(&mut self, file_name : &String){
        
        let byte_t = Symbol::new(".byte".to_string(), SymbolType::VarType(VarType::default()), false);
        let word_t = Symbol::new(".word".to_string(), SymbolType::VarType(VarType::default()), false);
        let string_t = Symbol::new(".string".to_string(), SymbolType::VarType(VarType::default()), false);
        
        let entry = self.symbol_table.entry(file_name.clone()).or_default();
        entry.push(byte_t);
        entry.push(word_t);
        entry.push(string_t);
    }

    pub fn new() -> SymbolTable {
        SymbolTable { symbol_table: HashMap::new() }
    }

    pub fn add_symbol(&mut self, file_name : String, symbol : &Rc<Symbol>) -> Option<AsmError> {

        if !self.symbol_table.contains_key(&file_name) {
            self.symbol_table.insert(file_name.clone(), Vec::new());
            self.insert_builtin_types(&file_name);
        }

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

    pub fn include_file_symbols(&mut self, source_file: &String, dest_file: String) -> Option<Vec<AsmError>>{

        let mut errors: Vec<AsmError> = Vec::new();
        if source_file == &dest_file {
            errors.push(AsmError::new(
                format!("File {} cannot include itself", dest_file),
                AsmErrorType::Include));
            return Some(errors);
        }
        let Some(source_table) = self.symbol_table.get(source_file) else{
            errors.push(AsmError::new(
                format!("Tried to included symbols from {} that doesn't exist", source_file), 
                AsmErrorType::Include));
            return Some(errors)
        };

        let symbols = source_table.clone();
        
        for symbol in &symbols {
            if symbol.is_public == false {
                continue;
            }

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


