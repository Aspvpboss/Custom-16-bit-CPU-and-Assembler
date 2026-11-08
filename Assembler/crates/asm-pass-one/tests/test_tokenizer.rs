use asm_pass_one::lexical_analysis::tokenizer::{self, next_token};

fn collect_tokens(mut source: String) -> Vec<String> {
    let mut tokens = Vec::new();
    while let Some((token, remaining)) = next_token(source) {
        tokens.push(token);
        source = remaining;
    }
    tokens
}

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

#[test]
fn empty_and_whitespace_only() {
    assert_eq!(next_token(String::from("")), None);
    // Whitespace-only input should return None (or no tokens)
    assert_eq!(next_token(String::from("   \n\t  ")), None);
}

#[test]
fn leading_and_trailing_whitespace() {
    let source = String::from("   \n\t  mov   r0   r1  \n  ");
    assert_eq!(collect_tokens(source), vec!["mov", "r0", "r1"]);
}

#[test]
fn labels_with_underscores_and_colons() {
    let source = String::from("label_A:\n  my_sub_routine_1:");
    assert_eq!(
        collect_tokens(source),
        vec!["label_A", ":", "my_sub_routine_1", ":"]
    );
}

#[test]
fn leading_underscore_label() {
    let source = String::from("_start: _loop");
    assert_eq!(
        collect_tokens(source),
        vec!["_start", ":", "_loop"]
    );
}

#[test]
fn trailing_whitespace_on_single_token() {
    let source = String::from("mov   \n\t  ");
    assert_eq!(
        collect_tokens(source),
        vec!["mov"]
    );
}

#[test]
fn single_token_with_leading_whitespace_only() {
    // e.g. the last token in a file preceded by spaces, with no trailing space
    let source = String::from("   mov");
    assert_eq!(
        collect_tokens(source),
        vec!["mov"]
    );
}


#[test]
fn immediates_and_negative_numbers() {
    let source = String::from("#10 #0xff #0b0000 #-127 3.14");
    assert_eq!(
        collect_tokens(source),
        vec!["#10", "#0xff", "#0b0000", "#-127", "3.14"]
    );
}

#[test]
fn directives_and_preprocessor_markers() {
    let source = String::from("@start test/main.asm\n.byte {#255 #4}\n.word .ascii .at #0x8000\n@end test/main.asm");
    assert_eq!(
        collect_tokens(source),
        vec![
            "@start", "test/main.asm",
            ".byte", "{", "#255", "#4", "}",
            ".word", ".ascii", ".at", "#0x8000",
            "@end", "test/main.asm"
        ]
    );
}

#[test]
fn string_literals_with_spaces() {
    let source = String::from("label: .ascii \"hello world\"");
    assert_eq!(
        collect_tokens(source),
        vec!["label", ":", ".ascii", "\"hello world\""]
    );
}

#[test]
fn all_special_characters_and_adjacent_tokens() {
    let source = String::from("{a}[b];:c");
    assert_eq!(
        collect_tokens(source),
        vec!["{", "a", "}", "[", "b", "]", ";", ":", "c"]
    );
}

#[test]
fn last_token_starts_with_non_alphanumeric() {
    // End of input reached while scanning a token starting with '#' or '.'
    let source = String::from("mov r0 #10");
    assert_eq!(
        collect_tokens(source),
        vec!["mov", "r0", "#10"]
    );
}

#[test]
fn special_char_followed_by_non_alphanumeric_token() {
    // Inside braces `{#255 #4}` without space after `{`, `prev_char` is `{` when `#` is read:
    let source = String::from("{#255 #4}");
    assert_eq!(
        collect_tokens(source),
        vec!["{", "#255", "#4", "}"]
    );
}


#[test]
fn newlines_and_tabs_between_tokens() {
    let source = String::from("add\tr1\nr2\r\nr3");
    assert_eq!(
        collect_tokens(source),
        vec!["add", "r1", "r2", "r3"]
    );
}

