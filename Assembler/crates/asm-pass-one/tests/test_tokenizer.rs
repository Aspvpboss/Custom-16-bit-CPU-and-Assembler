use asm_pass_one::lexical_analysis::tokenizer::{self, next_token};

#[test]
fn basic_test() {
    let source = String::from("Test Test Test");

    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "Test");
    assert!(!source.is_empty());
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "Test");
    assert!(!source.is_empty());
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "Test");
    assert!(source.is_empty());

}


#[test]
fn special_characters() {
    let source = String::from(";[r1];[r2];");

    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, ";");
    assert!(!source.is_empty());

    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "[");
    assert!(!source.is_empty());
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "r1");
    assert!(!source.is_empty());
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "]");
    
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, ";");  
    assert!(!source.is_empty());
  
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "[");
    assert!(!source.is_empty());
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "r2");
    assert!(!source.is_empty());
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, "]");
    
    let (token, source) = next_token(source).unwrap();
    assert_eq!(token, ";");
    assert!(source.is_empty());
    
}