use std::{rc::Rc};
use crate::asm::tokenize::{RawToken};

#[derive(Debug, Clone, PartialEq)]
pub enum LabelKind{
    Address,
    StringLit,
    Array,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Labels {
    kind: LabelKind,
    name: String,
    from_macro: bool,
}


#[derive(Debug, Clone, PartialEq)]
pub enum Instructions {
    // Memory
    Load = 0,
    Str,
    Push,
    Pop,
    Mov,

    // Control Flow
    Syscall,
    Jmp,
    Jif,
    Cal,
    Cif,
    Ret,

    // Integer ALU
    Add,
    Sub,
    Mul,
    Div,
    Mod,
    Cmp,
    And,
    Nor,
    Xor,
    Ars,
    Lrs,
    Lls,

    // Floating Point
    FAdd,
    FSub,
    FMul,
    FDiv,
    FSqrt,
    FCmp,
    FInt,
    IFlo,
}


#[derive(Debug, Clone, PartialEq)]
pub enum Identifiers {
    Instruction(Instructions),
    Reg(u8),
    WideReg(u8),
    FloatReg(u8),
    Label(Labels),
}

#[derive(Debug, Clone, PartialEq)]
pub enum Directives {
    Include,
    Allocate,
    Struct,
    VariableType(String),
    Glob,
    Macro,
}


#[derive(Debug, Clone, PartialEq)]
pub enum NumberTypes {
    Int(i32),
    Float(f32),
}


#[derive(Debug, Clone, PartialEq)]
pub enum RawLexedToken {
    Ident(Identifiers),
    Direct(Directives),
    Number(NumberTypes),
    StringLit(String),
    Unknown(String),
    FileStart,
    FileEnd,
    LCurly,
    RCurly,
    LBracket,
    RBracket,
    LParen,
    RParen,
    Quote,
    Colon,
    DoubleColon,
    Semicolon,
}

pub struct LexedToken{
    pub token: RawLexedToken,
    pub line: u32,
    pub column: u32,
    pub file_name: Rc<String>,
}

impl LexedToken{

    pub fn new(lex_type: RawLexedToken, og_token: &RawToken) -> LexedToken{
        LexedToken { 
            token: lex_type, 
            line: og_token.line, 
            column: og_token.column, 
            file_name: Rc::clone(&og_token.file_name) 
        }
    }

    pub fn from(lex_type: RawLexedToken, lexed_token: &LexedToken) -> LexedToken {
        LexedToken { 
            token: lex_type, 
            line: lexed_token.line, 
            column: lexed_token.column, 
            file_name: Rc::clone(&lexed_token.file_name) 
        }
    }


}

impl std::fmt::Display for LexedToken {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {

        write!(
            f,
            "{}:{}:{} - {:?}",
            self.file_name, self.line, self.column, self.token
        )
    }
}




fn lex_number_token(token_str: &str) -> Option<RawLexedToken> {
    let rest = token_str.strip_prefix('#')?;

    let (rest, negative) = match rest.strip_prefix('-') {
        Some(r) => (r, true),
        None => (rest, false),
    };

    // Floats: only plain decimal, no hex/bin float syntax.
    if rest.contains('.') {
        let value: f32 = rest.parse().ok()?;
        let value = if negative { -value } else { value };
        return Some(RawLexedToken::Number(NumberTypes::Float(value)));
    }

    let value: i32 = if let Some(hex) = rest.strip_prefix("0x").or_else(|| rest.strip_prefix("0X")) {
        i32::from_str_radix(hex, 16).ok()?
    } else if let Some(bin) = rest.strip_prefix("0b").or_else(|| rest.strip_prefix("0B")) {
        i32::from_str_radix(bin, 2).ok()?
    } else {
        rest.parse::<i32>().ok()?
    };

    let value = if negative { -value } else { value };
    Some(RawLexedToken::Number(NumberTypes::Int(value)))
}

fn lex_identifiers_token(token_str: &str) -> Option<RawLexedToken> {
    if let Some(num_str) = token_str.strip_prefix("wr") {
        let reg_num: u8 = num_str.parse().ok()?;
        return Some(RawLexedToken::Ident(Identifiers::WideReg(reg_num)));
    }
    if let Some(num_str) = token_str.strip_prefix("fr") {
        let reg_num: u8 = num_str.parse().ok()?;
        return Some(RawLexedToken::Ident(Identifiers::FloatReg(reg_num)));
    }
    if let Some(num_str) = token_str.strip_prefix('r') {
        let reg_num: u8 = num_str.parse().ok()?;
        return Some(RawLexedToken::Ident(Identifiers::Reg(reg_num)));
    }

    None
}


fn lex_instruction_token(token_str: &str) -> Option<RawLexedToken> {
    match token_str.to_lowercase().as_str() {
        // Memory
        "load" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Load))),
        "str"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Str))),
        "push" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Push))),
        "pop"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Pop))),
        "mov"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Mov))),

        // Control Flow
        "syscall" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Syscall))),
        "jmp"     => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Jmp))),
        "jif"     => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Jif))),
        "cal"     => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Cal))),
        "cif"     => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Cif))),
        "ret"     => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Ret))),

        // Integer ALU
        "add" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Add))),
        "sub" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Sub))),
        "mul" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Mul))),
        "div" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Div))),
        "mod" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Mod))),
        "cmp" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Cmp))),
        "and" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::And))),
        "nor" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Nor))),
        "xor" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Xor))),
        "ars" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Ars))),
        "lrs" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Lrs))),
        "lls" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::Lls))),

        // Floating Point
        "fadd"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::FAdd))),
        "fsub"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::FSub))),
        "fmul"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::FMul))),
        "fdiv"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::FDiv))),
        "fsqrt" => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::FSqrt))),
        "fcmp"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::FCmp))),
        "fint"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::FInt))),
        "iflo"  => Some(RawLexedToken::Ident(Identifiers::Instruction(Instructions::IFlo))),

        _ => None
    }
}

fn lex_directives_token(token_str: &str) -> Option<RawLexedToken> {
    match token_str {

        ".include" => Some(RawLexedToken::Direct(Directives::Include)),
        ".allocate" => Some(RawLexedToken::Direct(Directives::Allocate)),
        ".struct" => Some(RawLexedToken::Direct(Directives::Struct)),
        ".macro" => Some(RawLexedToken::Direct(Directives::Macro)),
        ".glob" => Some(RawLexedToken::Direct(Directives::Glob)),

        _ => None
    } 
}

fn lex_easy_tokens(token_str: &str) -> Option<RawLexedToken>{
    match token_str {
        ".file_start" => Some(RawLexedToken::FileStart),
        ".file_end" => Some(RawLexedToken::FileEnd),
        ":" => Some(RawLexedToken::Colon),
        "::" => Some(RawLexedToken::DoubleColon),
        ";" => Some(RawLexedToken::Semicolon),
        "{" => Some(RawLexedToken::LCurly),
        "}" => Some(RawLexedToken::RCurly),
        "[" => Some(RawLexedToken::LBracket),
        "]" => Some(RawLexedToken::RBracket),
        "(" => Some(RawLexedToken::LParen),
        ")" => Some(RawLexedToken::RParen),
        "\"" => Some(RawLexedToken::Quote),
        _ => {None}    
    }
}


fn simple_lex(token: &RawToken) -> LexedToken {

    let token_str = token.raw_token.as_str();
    let mut lex_type: Option<RawLexedToken> = None;

    if lex_type.is_none() {
        lex_type = lex_easy_tokens(token_str);      
    }
    if lex_type.is_none() {
        lex_type = lex_instruction_token(token_str);
    }
    if lex_type.is_none() {
        lex_type = lex_directives_token(token_str);
    }
    if lex_type.is_none() {
        lex_type = lex_identifiers_token(token_str);
    }
    if lex_type.is_none() {
        lex_type = lex_number_token(token_str);
    }


    match lex_type{
        Some(lex_type) => LexedToken::new(lex_type, token),
        None => LexedToken::new(RawLexedToken::Unknown(token.raw_token.clone()), token)
    } 
}


fn lex_string_literal(tokens: &Vec<RawToken>, i: usize) -> Option<LexedToken> {
    if i + 2 >= tokens.len() { return None; }
    if tokens[i].raw_token != "\"" { return None; }
    if tokens[i + 2].raw_token != "\"" { return None; }

    let mut string_lit = String::new();
    let mut chars = tokens[i + 1].raw_token.chars();
    while let Some(ch) = chars.next() {
        if ch == '\\' {
            match chars.next() {
                Some('n') => string_lit.push('\n'),
                Some('t') => string_lit.push('\t'),
                Some('\\') => string_lit.push('\\'),
                Some('"') => string_lit.push('"'),
                Some(other) => {
                    string_lit.push('\\');
                    string_lit.push(other);
                }
                None => string_lit.push('\\'),
            }
        } else {
            string_lit.push(ch);
        }
    }


    Some(LexedToken::new(
        RawLexedToken::StringLit(string_lit),
        &tokens[i], // position at the opening quote
    ))
}


pub fn first_pass_lexer(tokens: Vec<RawToken>) -> Vec<LexedToken> {
    let mut lexed_tokens: Vec<LexedToken> = Vec::new();

    let mut i = 0;
    while i < tokens.len() {
        if let Some(string_lit_token) = lex_string_literal(&tokens, i) {
            lexed_tokens.push(string_lit_token);
            i += 3;
            continue;
        }

        let lexed_token = simple_lex(&tokens[i]);
        lexed_tokens.push(lexed_token);
        i += 1;
    }

    lexed_tokens
}



