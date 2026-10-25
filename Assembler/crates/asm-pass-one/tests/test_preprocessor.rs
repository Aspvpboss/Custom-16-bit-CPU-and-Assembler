use asm_cli::user_args::{UserArgs, UserFlags};
use asm_core::error::{AsmError, AsmErrorType};
use std::env;
use std::fs;
use std::path::PathBuf;
use std::sync::Mutex;
use std::sync::atomic::{AtomicUsize, Ordering};

static TEST_COUNTER: AtomicUsize = AtomicUsize::new(0);
static DUMP_FILE_LOCK: Mutex<()> = Mutex::new(());

struct TempTestDir {
    path: PathBuf,
}

impl TempTestDir {
    fn new(test_name: &str) -> Self {
        let id = TEST_COUNTER.fetch_add(1, Ordering::SeqCst);
        let mut path = env::temp_dir();
        path.push(format!(
            "rose_asm_prepro_test_{}_{}_{}",
            std::process::id(),
            test_name,
            id
        ));
        fs::create_dir_all(&path).expect("failed to create temp test dir");
        Self { path }
    }

    fn write_file(&self, relative_name: &str, contents: &str) -> String {
        let file_path = self.path.join(relative_name);
        if let Some(parent) = file_path.parent() {
            fs::create_dir_all(parent).expect("failed to create parent dirs");
        }
        fs::write(&file_path, contents).expect("failed to write test asm file");
        file_path.to_string_lossy().to_string()
    }

    fn dir_with_slash(&self) -> String {
        format!("{}/", self.path.to_string_lossy())
    }

    fn make_args(&self, files: &[(&str, &str)], flags: Vec<UserFlags>) -> UserArgs {
        let mut src_files = Vec::new();
        for (name, content) in files {
            src_files.push(self.write_file(name, content));
        }
        UserArgs {
            src_files,
            user_flags: flags,
            output_file: "out.bin".to_string(),
            src_directory: self.dir_with_slash(),
        }
    }
}

impl Drop for TempTestDir {
    fn drop(&mut self) {
        let _ = fs::remove_dir_all(&self.path);
    }
}

fn unwrap_ok(res: Result<String, Vec<AsmError>>) -> String {
    match res {
        Ok(output) => output,
        Err(errs) => {
            let formatted: Vec<String> = errs.iter().map(|e| e.to_string()).collect();
            panic!("Expected Ok from preprocessor, got Err: {:?}", formatted);
        }
    }
}

fn unwrap_err(res: Result<String, Vec<AsmError>>) -> Vec<AsmError> {
    match res {
        Ok(output) => panic!("Expected Err from preprocessor, got Ok: {output:?}"),
        Err(errs) => errs,
    }
}

fn extract_prepro_errors(errors: Vec<AsmError>) -> Vec<String> {
    errors
        .into_iter()
        .map(|err| {
            assert!(
                err.src_location.is_none(),
                "Preprocessor errors should have None src_location by default"
            );
            match err.err_type {
                AsmErrorType::PREPRO(ref msg) => {
                    assert_eq!(err.to_string(), format!("Preprocessor Error: {msg}"));
                    msg.clone()
                }
                other => panic!("Expected AsmErrorType::PREPRO, got: {other}"),
            }
        })
        .collect()
}

// ============================================================================
// 1. Basic File Wrapping (@start / @end Directives)
// ============================================================================

#[test]
fn test_single_main_file_start_and_end_directives() {
    let dir = TempTestDir::new("single_main");
    let args = dir.make_args(&[("main.asm", "add r0 r1 r2\nsub r3 r4 r5")], vec![]);

    let main_path = &args.src_files[0];
    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));

    let expected = format!("@start {main_path}\nadd r0 r1 r2\nsub r3 r4 r5\n@end {main_path}\n");
    assert_eq!(output, expected);
}

#[test]
fn test_top_level_pass_one_run_ok() {
    let dir = TempTestDir::new("pass_one_run");
    let args = dir.make_args(&[("main.asm", "mov r0 r1")], vec![]);

    let res = asm_pass_one::run(&args);
    assert!(res.is_ok());
}

// ============================================================================
// 2. @include Resolution
// ============================================================================

#[test]
fn test_single_include_resolution() {
    let dir = TempTestDir::new("single_include");
    let args = dir.make_args(
        &[
            ("main.asm", "line_before\n@include \"helper.asm\"\nline_after"),
            ("helper.asm", "helper_line_1\nhelper_line_2"),
        ],
        vec![],
    );

    let main_path = &args.src_files[0];
    let helper_path = &args.src_files[1];

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    let expected = format!(
        "@start {main_path}\nline_before\n@start {helper_path}\nhelper_line_1\nhelper_line_2\n@end {helper_path}\nline_after\n@end {main_path}\n"
    );
    assert_eq!(output, expected);
}

#[test]
fn test_multiple_includes_with_extra_whitespace() {
    let dir = TempTestDir::new("multi_include");
    let args = dir.make_args(
        &[
            (
                "main.asm",
                "1\n@include     \"foo1.asm\"\nmiddle\n@include \"burger.asm\"",
            ),
            ("foo1.asm", "4\n5"),
            ("burger.asm", "7\n8"),
        ],
        vec![],
    );

    let main_path = &args.src_files[0];
    let foo_path = &args.src_files[1];
    let burger_path = &args.src_files[2];

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    let expected = format!(
        "@start {main_path}\n1\n@start {foo_path}\n4\n5\n@end {foo_path}\nmiddle\n@start {burger_path}\n7\n8\n@end {burger_path}\n@end {main_path}\n"
    );
    assert_eq!(output, expected);
}

#[test]
fn test_nested_includes() {
    let dir = TempTestDir::new("nested_include");
    let args = dir.make_args(
        &[
            ("main.asm", "start_main\n@include \"a.asm\"\nend_main"),
            ("a.asm", "start_a\n@include \"b.asm\"\nend_a"),
            ("b.asm", "inside_b"),
        ],
        vec![],
    );

    let main_path = &args.src_files[0];
    let a_path = &args.src_files[1];
    let b_path = &args.src_files[2];

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    let expected = format!(
        "@start {main_path}\nstart_main\n@start {a_path}\nstart_a\n@start {b_path}\ninside_b\n@end {b_path}\nend_a\n@end {a_path}\nend_main\n@end {main_path}\n"
    );
    assert_eq!(output, expected);
}

#[test]
fn test_duplicate_include_is_only_included_once() {
    let dir = TempTestDir::new("dup_include");
    let args = dir.make_args(
        &[
            (
                "main.asm",
                "@include \"common.asm\"\nmiddle\n@include \"common.asm\"",
            ),
            ("common.asm", "shared_code"),
        ],
        vec![],
    );

    let main_path = &args.src_files[0];
    let common_path = &args.src_files[1];

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    // Second @include "common.asm" is skipped because common.asm was already included
    let expected = format!(
        "@start {main_path}\n@start {common_path}\nshared_code\n@end {common_path}\nmiddle\n@end {main_path}\n"
    );
    assert_eq!(output, expected);
}

#[test]
fn test_circular_include_terminates_cleanly() {
    let dir = TempTestDir::new("circular_include");
    let args = dir.make_args(
        &[
            ("main.asm", "in_main\n@include \"a.asm\""),
            ("a.asm", "in_a\n@include \"b.asm\""),
            ("b.asm", "in_b\n@include \"a.asm\""),
        ],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(output.contains("in_main"));
    assert!(output.contains("in_a"));
    assert!(output.contains("in_b"));
}

// ============================================================================
// 3. Comment Removal (Single-Line // and Multi-Line /* ... */)
// ============================================================================

#[test]
fn test_single_line_comments_removed() {
    let dir = TempTestDir::new("single_comments");
    let args = dir.make_args(
        &[(
            "main.asm",
            "// full line comment\nadd r0 r1 r2 // trailing comment\nsub r3 r4 r5",
        )],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(!output.contains("full line comment"));
    assert!(!output.contains("trailing comment"));
    assert!(!output.contains("//"));
    assert!(output.contains("add r0 r1 r2"));
    assert!(output.contains("sub r3 r4 r5"));
}

#[test]
fn test_inline_multi_line_comment_masked_with_spaces() {
    let dir = TempTestDir::new("inline_block_comment");
    let args = dir.make_args(
        &[("main.asm", "burger /* hidden */ after")],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(!output.contains("hidden"));
    assert!(!output.contains("/*"));
    assert!(!output.contains("*/"));
    assert!(output.contains("burger"));
    assert!(output.contains("after"));
}

#[test]
fn test_spanning_multi_line_comment_removed() {
    let dir = TempTestDir::new("spanning_block_comment");
    let args = dir.make_args(
        &[(
            "main.asm",
            "before_block\n/* start comment\nmiddle comment line 1\nmiddle comment line 2\nend comment */\nafter_block",
        )],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(output.contains("before_block"));
    assert!(output.contains("after_block"));
    assert!(!output.contains("start comment"));
    assert!(!output.contains("middle comment"));
    assert!(!output.contains("end comment"));
    assert!(!output.contains("/*"));
    assert!(!output.contains("*/"));
}

// ============================================================================
// 4. Macro Definition and Expansion (@macro)
// ============================================================================

#[test]
fn test_macro_definition_and_expansion_with_commas() {
    let dir = TempTestDir::new("macro_expand");
    let args = dir.make_args(
        &[(
            "main.asm",
            "@macro REG_POINTER r3\n@macro #PI #3.141528\nadd r0 REG_POINTER, #PI",
        )],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(!output.contains("@macro"));
    assert!(!output.contains("REG_POINTER"));
    assert!(!output.contains("#PI"));
    assert!(output.contains("add r0 r3  #3.141528"));
}

#[test]
fn test_macro_defined_in_included_file_expands_in_main() {
    let dir = TempTestDir::new("macro_across_include");
    let args = dir.make_args(
        &[
            ("main.asm", "@include \"defs.asm\"\nmov SRC_REG, DST_REG"),
            ("defs.asm", "@macro SRC_REG r1\n@macro DST_REG r2"),
        ],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(!output.contains("SRC_REG"));
    assert!(!output.contains("DST_REG"));
    assert!(output.contains("mov r1  r2"));
}

#[test]
fn test_duplicate_macro_keeps_first_definition() {
    let dir = TempTestDir::new("duplicate_macro");
    let args = dir.make_args(
        &[(
            "main.asm",
            "@macro TARGET r1\n@macro TARGET r7\nmov r0 TARGET",
        )],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(output.contains("mov r0 r1"));
    assert!(!output.contains("mov r0 r7"));
}

#[test]
fn test_macro_not_expanded_before_definition() {
    let dir = TempTestDir::new("macro_order");
    let args = dir.make_args(
        &[(
            "main.asm",
            "mov r0 LATER_MACRO\n@macro LATER_MACRO r5\nmov r1 LATER_MACRO",
        )],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(output.contains("mov r0 LATER_MACRO"));
    assert!(output.contains("mov r1 r5"));
}

// ============================================================================
// 5. DumpPreprocessor Flag (PREPROCESSOR_DUMP.txt)
// ============================================================================

#[test]
fn test_dump_preprocessor_flag_writes_file() {
    let _guard = DUMP_FILE_LOCK.lock().expect("failed to lock dump mutex");
    let dump_path = PathBuf::from("PREPROCESSOR_DUMP.txt");
    let _ = fs::remove_file(&dump_path);

    let dir = TempTestDir::new("dump_flag");
    let args = dir.make_args(
        &[("main.asm", "@macro TEN #10\nadd r0 r1, TEN // add ten\n")],
        vec![UserFlags::DumpPreprocessor],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    let dumped = fs::read_to_string(&dump_path).expect("PREPROCESSOR_DUMP.txt should be created");
    assert_eq!(dumped, output);

    let _ = fs::remove_file(&dump_path);
}

// ============================================================================
// 6. Error Handling (AsmErrorType::PREPRO)
// ============================================================================

#[test]
fn test_err_missing_source_file_on_disk() {
    let dir = TempTestDir::new("err_missing_disk");
    let main_path = dir.write_file("main.asm", "add r0 r1 r2");
    let nonexistent = format!("{}does_not_exist.asm", dir.dir_with_slash());

    let args = UserArgs {
        src_files: vec![main_path, nonexistent.clone()],
        user_flags: vec![],
        output_file: "out.bin".to_string(),
        src_directory: dir.dir_with_slash(),
    };

    let err = unwrap_err(asm_pass_one::preprocessor::run(&args));
    let msgs = extract_prepro_errors(err);
    assert_eq!(
        msgs,
        vec![format!("failed to open source file {nonexistent}")]
    );
}

#[test]
fn test_err_missing_root_main_asm() {
    let dir = TempTestDir::new("err_no_main");
    let args = dir.make_args(&[("other.asm", "add r0 r1 r2")], vec![]);

    let err = unwrap_err(asm_pass_one::preprocessor::run(&args));
    let msgs = extract_prepro_errors(err);
    assert_eq!(msgs, vec!["root file main.asm not found".to_string()]);
}

#[test]
fn test_err_include_file_not_in_src_files() {
    let dir = TempTestDir::new("err_unlisted_include");
    let args = dir.make_args(
        &[("main.asm", "@include \"unlisted.asm\"\nadd r0 r1 r2")],
        vec![],
    );

    let expected_missing = format!("{}unlisted.asm", dir.dir_with_slash());
    let err = unwrap_err(asm_pass_one::preprocessor::run(&args));
    let msgs = extract_prepro_errors(err);
    assert_eq!(msgs, vec![format!("Failed to include {expected_missing}")]);
}

#[test]
fn test_err_multiple_preprocessor_errors_accumulated() {
    let dir = TempTestDir::new("err_multiple");
    let main_path = dir.write_file(
        "main.asm",
        "@include \"missing_inc1.asm\"\n@include \"missing_inc2.asm\"",
    );
    let bad_src = format!("{}ghost.asm", dir.dir_with_slash());

    let args = UserArgs {
        src_files: vec![main_path, bad_src.clone()],
        user_flags: vec![],
        output_file: "out.bin".to_string(),
        src_directory: dir.dir_with_slash(),
    };

    let err = unwrap_err(asm_pass_one::preprocessor::run(&args));
    let msgs = extract_prepro_errors(err);
    assert_eq!(
        msgs,
        vec![
            format!("failed to open source file {bad_src}"),
            format!("Failed to include {}missing_inc1.asm", dir.dir_with_slash()),
            format!("Failed to include {}missing_inc2.asm", dir.dir_with_slash()),
        ]
    );
}

// ============================================================================
// 7. Strict Edge-Case Tests (Comments, Macros, Includes, Malformed Directives)
// ============================================================================

#[test]
fn test_commented_out_single_line_include_is_ignored() {
    let dir = TempTestDir::new("commented_single_include");
    let args = dir.make_args(
        &[
            (
                "main.asm",
                "// @include \"never.asm\"\n@include \"excluded.asm\" // trailing\n",
            ),
            ("excluded.asm", "included_line"),
        ],
        vec![],
    );

    // `// @include "never.asm"` is commented out, so it must NOT fail trying to include `never.asm`
    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(!output.contains("never.asm"));
    assert!(output.contains("included_line"));
}

#[test]
fn test_commented_out_multi_line_include_is_ignored() {
    let dir = TempTestDir::new("commented_multi_include");
    let args = dir.make_args(
        &[
            (
                "main.asm",
                "/*\n@include \"ghost.asm\"\n@include \"secret.asm\"\n*/\nadd r0 r1 r2",
            ),
            ("secret.asm", "secret_payload"),
        ],
        vec![],
    );

    // Neither ghost.asm nor secret.asm should be included when inside a /* ... */ block
    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(!output.contains("secret_payload"));
    assert!(output.contains("add r0 r1 r2"));
}

#[test]
fn test_macro_expansion_does_not_corrupt_substring_tokens() {
    let dir = TempTestDir::new("macro_substring_collision");
    let args = dir.make_args(
        &[(
            "main.asm",
            "@macro r1 r2\n@macro A B\nadd r10, r1\njmp LABEL_A, A",
        )],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    // `r10` must remain `r10` (not `r20`), and `LABEL_A` must remain `LABEL_A` (not `LABEL_B`)
    assert!(
        output.contains("r10"),
        "Macro `r1 -> r2` corrupted `r10` into `r20`: {output:?}"
    );
    assert!(
        !output.contains("r20"),
        "Macro `r1 -> r2` corrupted `r10` into `r20`: {output:?}"
    );
    assert!(
        output.contains("LABEL_A"),
        "Macro `A -> B` corrupted `LABEL_A` into `LABEL_B`: {output:?}"
    );
    assert!(
        !output.contains("LABEL_B"),
        "Macro `A -> B` corrupted `LABEL_A` into `LABEL_B`: {output:?}"
    );
}

#[test]
fn test_single_line_comment_inside_multiline_comment_does_not_eat_closing_delimiter() {
    let dir = TempTestDir::new("slash_slash_in_block_comment");
    let args = dir.make_args(
        &[(
            "main.asm",
            "/* block start\n// comment inside block with closing delimiter */\nadd r0 r1 r2",
        )],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(
        output.contains("add r0 r1 r2"),
        "`//` inside `/* ... */` prevented `*/` from closing the block comment: {output:?}"
    );
}

#[test]
fn test_multiple_inline_block_comments_on_same_line() {
    let dir = TempTestDir::new("multiple_inline_block_comments");
    let args = dir.make_args(
        &[(
            "main.asm",
            "/* first */ add r0 /* second */ r1 /* third */ r2",
        )],
        vec![],
    );

    let output = unwrap_ok(asm_pass_one::preprocessor::run(&args));
    assert!(!output.contains("first"));
    assert!(
        !output.contains("second"),
        "Second inline block comment was not removed: {output:?}"
    );
    assert!(
        !output.contains("third"),
        "Third inline block comment was not removed: {output:?}"
    );
    assert!(!output.contains("/*"));
    assert!(!output.contains("*/"));
    assert!(output.contains("add r0"));
    assert!(output.contains("r1"));
    assert!(output.contains("r2"));
}

#[test]
fn test_err_unclosed_multiline_comment() {
    let dir = TempTestDir::new("err_unclosed_block_comment");
    let args = dir.make_args(
        &[("main.asm", "add r0 r1 r2\n/* unclosed comment\nsub r3 r4 r5")],
        vec![],
    );

    let err = unwrap_err(asm_pass_one::preprocessor::run(&args));
    let msgs = extract_prepro_errors(err);
    assert!(
        !msgs.is_empty(),
        "Expected a preprocessor error for unclosed `/*` comment"
    );
}

#[test]
fn test_err_malformed_include_missing_quotes() {
    let dir = TempTestDir::new("err_malformed_include");
    let args = dir.make_args(
        &[
            ("main.asm", "@include foo.asm\nadd r0 r1 r2"),
            ("foo.asm", "sub r1 r2 r3"),
        ],
        vec![],
    );

    let err = unwrap_err(asm_pass_one::preprocessor::run(&args));
    let msgs = extract_prepro_errors(err);
    assert!(
        !msgs.is_empty(),
        "Expected a preprocessor error for malformed `@include` without quotes"
    );
}

#[test]
fn test_err_malformed_macro_missing_arguments() {
    let dir = TempTestDir::new("err_malformed_macro");
    let args = dir.make_args(
        &[("main.asm", "@macro ONLY_NAME\nadd r0 r1 r2")],
        vec![],
    );

    let err = unwrap_err(asm_pass_one::preprocessor::run(&args));
    let msgs = extract_prepro_errors(err);
    assert!(
        !msgs.is_empty(),
        "Expected a preprocessor error for `@macro` missing replacement value"
    );
}

