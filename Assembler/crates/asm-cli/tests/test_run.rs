use asm_core::error::AsmErrorType;
use std::env;
use std::process::{Command, exit};

// Intercept execution when running in child test mode before libtest runner parses args.
// This allows testing `asm_cli::run()` with arbitrary command-line arguments.
#[cfg(target_os = "linux")]
#[used]
#[unsafe(link_section = ".init_array")]
static INIT_LINUX: extern "C" fn() = child_init;

#[cfg(target_vendor = "apple")]
#[used]
#[unsafe(link_section = "__DATA,__mod_init_func")]
static INIT_MACOS: extern "C" fn() = child_init;

#[cfg(target_os = "windows")]
#[used]
#[unsafe(link_section = ".CRT$XCU")]
static INIT_WINDOWS: extern "C" fn() = child_init;

extern "C" fn child_init() {
    if env::var("ASM_CLI_TEST_CHILD").is_ok() {
        println!("===ASM_CLI_TEST_RESULT_START===");
        match asm_cli::run() {
            Ok(user_args) => {
                println!("OK");
                println!("OUT:{}", user_args.output_file);
                for src in user_args.src_files {
                    println!("SRC:{}", src);
                }
            }
            Err(errors) => {
                println!("ERR");
                for err in errors {
                    // Verify error invariant: CLI errors should not have a source file location.
                    assert!(
                        err.src_location.is_none(),
                        "CLI errors should have None src_location"
                    );
                    match err.err_type {
                        AsmErrorType::CLI(ref msg) => {
                            // Verify Display implementation outputs "CLI Error: {msg}"
                            assert_eq!(err.to_string(), format!("CLI Error: {msg}"));
                            println!("CLI_ERR:{}", msg);
                        }
                    }
                }
            }
        }
        println!("===ASM_CLI_TEST_RESULT_END===");
        exit(0);
    }
}

#[derive(Debug, PartialEq, Eq)]
struct TestResult {
    success: bool,
    output_file: String,
    src_files: Vec<String>,
    errors: Vec<String>,
}

impl TestResult {
    fn is_ok(&self) -> bool {
        self.success
    }

    fn is_err(&self) -> bool {
        !self.success
    }
}

fn parse_result(output: &str) -> TestResult {
    let mut in_block = false;
    let mut success = false;
    let mut output_file = String::new();
    let mut src_files = Vec::new();
    let mut errors = Vec::new();

    for line in output.lines() {
        if line == "===ASM_CLI_TEST_RESULT_START===" {
            in_block = true;
            continue;
        }
        if line == "===ASM_CLI_TEST_RESULT_END===" {
            break;
        }
        if !in_block {
            continue;
        }

        if line == "OK" {
            success = true;
        } else if line == "ERR" {
            success = false;
        } else if let Some(out) = line.strip_prefix("OUT:") {
            output_file = out.to_string();
        } else if let Some(src) = line.strip_prefix("SRC:") {
            src_files.push(src.to_string());
        } else if let Some(err) = line.strip_prefix("CLI_ERR:") {
            errors.push(err.to_string());
        }
    }

    TestResult {
        success,
        output_file,
        src_files,
        errors,
    }
}

fn run_asm_cli(args: &[&str]) -> TestResult {
    let current_exe = env::current_exe().expect("failed to get current_exe");
    let output = Command::new(current_exe)
        .env("ASM_CLI_TEST_CHILD", "1")
        .args(args)
        .output()
        .expect("failed to execute child process");

    assert!(output.status.success(), "child process failed: {:?}", output);
    let stdout = String::from_utf8_lossy(&output.stdout);
    parse_result(&stdout)
}

// ============================================================================
// 1. Successful CLI Argument Combinations
// ============================================================================

#[test]
fn test_success_single_source_and_output() {
    let result = run_asm_cli(&["input.asm", "-o", "output.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["input.asm"]);
    assert_eq!(result.output_file, "output.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_reversed_flag_order() {
    let result = run_asm_cli(&["-o", "output.bin", "input.asm"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["input.asm"]);
    assert_eq!(result.output_file, "output.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_multiple_source_files() {
    let result = run_asm_cli(&["a.asm", "b.asm", "c.asm", "-o", "result.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["a.asm", "b.asm", "c.asm"]);
    assert_eq!(result.output_file, "result.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_interleaved_flags_and_sources() {
    let result = run_asm_cli(&["first.asm", "-o", "result.bin", "second.asm"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["first.asm", "second.asm"]);
    assert_eq!(result.output_file, "result.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_with_source_directory() {
    let result = run_asm_cli(&["-S", "sources", "main.asm", "-o", "output.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["sources/main.asm"]);
    assert_eq!(result.output_file, "output.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_with_source_directory_and_multiple_files() {
    let result = run_asm_cli(&["-S", "src/asm", "foo.asm", "bar.asm", "-o", "out.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["src/asm/foo.asm", "src/asm/bar.asm"]);
    assert_eq!(result.output_file, "out.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_source_dir_at_end() {
    let result = run_asm_cli(&["main.asm", "-o", "out.bin", "-S", "my_dir"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["my_dir/main.asm"]);
    assert_eq!(result.output_file, "out.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_source_dir_in_middle() {
    let result = run_asm_cli(&["a.asm", "-S", "dir", "b.asm", "-o", "out.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["dir/a.asm", "dir/b.asm"]);
    assert_eq!(result.output_file, "out.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_source_dir_with_trailing_slash() {
    let result = run_asm_cli(&["-S", "my_dir/", "main.asm", "-o", "out.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["my_dir//main.asm"]);
    assert_eq!(result.output_file, "out.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_source_dir_empty_string() {
    let result = run_asm_cli(&["-S", "", "main.asm", "-o", "out.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["/main.asm"]);
    assert_eq!(result.output_file, "out.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_dot_asm_filename() {
    let result = run_asm_cli(&[".asm", "-o", "out.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec![".asm"]);
    assert_eq!(result.output_file, "out.bin");
    assert!(result.errors.is_empty());
}

#[test]
fn test_success_nested_paths() {
    let result = run_asm_cli(&["dir/sub/file.asm", "-o", "build/out.bin"]);
    assert!(result.is_ok());
    assert_eq!(result.src_files, vec!["dir/sub/file.asm"]);
    assert_eq!(result.output_file, "build/out.bin");
    assert!(result.errors.is_empty());
}

// ============================================================================
// 2. Missing Required Arguments
// ============================================================================

#[test]
fn test_err_no_args() {
    let result = run_asm_cli(&[]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["No source files given", "No output file given"]
    );
}

#[test]
fn test_err_missing_output_file() {
    let result = run_asm_cli(&["main.asm"]);
    assert!(result.is_err());
    assert_eq!(result.errors, vec!["No output file given"]);
}

#[test]
fn test_err_missing_source_files() {
    let result = run_asm_cli(&["-o", "out.bin"]);
    assert!(result.is_err());
    assert_eq!(result.errors, vec!["No source files given"]);
}

#[test]
fn test_err_missing_source_files_with_source_directory() {
    let result = run_asm_cli(&["-S", "src", "-o", "out.bin"]);
    assert!(result.is_err());
    assert_eq!(result.errors, vec!["No source files given"]);
}

#[test]
fn test_err_missing_output_file_with_source_directory() {
    let result = run_asm_cli(&["-S", "src", "main.asm"]);
    assert!(result.is_err());
    assert_eq!(result.errors, vec!["No output file given"]);
}

// ============================================================================
// 3. Open-Ended Subcommands
// ============================================================================

#[test]
fn test_err_open_ended_output_flag() {
    let result = run_asm_cli(&["main.asm", "-o"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["opened ended sub-command", "No output file given"]
    );
}

#[test]
fn test_err_open_ended_source_dir_flag() {
    let result = run_asm_cli(&["main.asm", "-o", "out.bin", "-S"]);
    assert!(result.is_err());
    assert_eq!(result.errors, vec!["opened ended sub-command"]);
}

#[test]
fn test_err_only_open_ended_output_flag() {
    let result = run_asm_cli(&["-o"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "opened ended sub-command",
            "No source files given",
            "No output file given"
        ]
    );
}

#[test]
fn test_err_only_open_ended_source_dir_flag() {
    let result = run_asm_cli(&["-S"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "opened ended sub-command",
            "No source files given",
            "No output file given"
        ]
    );
}

// ============================================================================
// 4. Duplicate Flags
// ============================================================================

#[test]
fn test_err_duplicate_output_flag() {
    let result = run_asm_cli(&["main.asm", "-o", "out1.bin", "-o", "out2.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "duplicate arg out1.bin",
            "invalid file extension bin for file out2.bin. Valid file extensions are [\"asm\"]"
        ]
    );
}

#[test]
fn test_err_duplicate_output_flag_followed_by_valid_asm() {
    let result = run_asm_cli(&["main.asm", "-o", "out.bin", "-o", "extra.asm"]);
    assert!(result.is_err());
    assert_eq!(result.errors, vec!["duplicate arg out.bin"]);
}

#[test]
fn test_err_duplicate_source_dir_flag() {
    let result = run_asm_cli(&["-S", "dir1", "-S", "dir2", "main.asm", "-o", "out.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "duplicate arg dir1",
            "failed to get file_extension for file dir2"
        ]
    );
}

#[test]
fn test_err_duplicate_both_flags() {
    let result = run_asm_cli(&[
        "main.asm",
        "-o",
        "out1.bin",
        "-o",
        "out2.bin",
        "-S",
        "dir1",
        "-S",
        "dir2",
    ]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "duplicate arg out1.bin",
            "invalid file extension bin for file out2.bin. Valid file extensions are [\"asm\"]",
            "duplicate arg dir1",
            "failed to get file_extension for file dir2"
        ]
    );
}

#[test]
fn test_err_triple_duplicate_output_flag() {
    let result = run_asm_cli(&["main.asm", "-o", "out1.bin", "-o", "out2.bin", "-o", "out3.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "duplicate arg out1.bin",
            "invalid file extension bin for file out2.bin. Valid file extensions are [\"asm\"]",
            "duplicate arg out1.bin",
            "invalid file extension bin for file out3.bin. Valid file extensions are [\"asm\"]"
        ]
    );
}

#[test]
fn test_err_triple_duplicate_source_dir_flag() {
    let result = run_asm_cli(&[
        "-S", "dir1", "-S", "dir2", "-S", "dir3", "main.asm", "-o", "out.bin",
    ]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "duplicate arg dir1",
            "failed to get file_extension for file dir2",
            "duplicate arg dir1",
            "failed to get file_extension for file dir3"
        ]
    );
}

// ============================================================================
// 5. Invalid File Extensions
// ============================================================================

#[test]
fn test_err_source_file_invalid_extension() {
    let result = run_asm_cli(&["main.txt", "-o", "out.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "invalid file extension txt for file main.txt. Valid file extensions are [\"asm\"]",
            "No source files given"
        ]
    );
}

#[test]
fn test_err_source_file_missing_extension() {
    let result = run_asm_cli(&["main", "-o", "out.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "failed to get file_extension for file main",
            "No source files given"
        ]
    );
}

#[test]
fn test_err_source_file_multiple_dots() {
    let result = run_asm_cli(&["main.test.asm", "-o", "out.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "invalid file extension test for file main.test.asm. Valid file extensions are [\"asm\"]",
            "No source files given"
        ]
    );
}

#[test]
fn test_err_output_file_invalid_extension() {
    let result = run_asm_cli(&["main.asm", "-o", "out.txt"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["invalid file extension txt for file out.txt. Valid file extensions are [\"bin\"]"]
    );
}

#[test]
fn test_err_output_file_missing_extension() {
    let result = run_asm_cli(&["main.asm", "-o", "out"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["failed to get file_extension for file out"]
    );
}

#[test]
fn test_err_output_file_multiple_dots() {
    let result = run_asm_cli(&["main.asm", "-o", "out.test.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["invalid file extension test for file out.test.bin. Valid file extensions are [\"bin\"]"]
    );
}

#[test]
fn test_err_output_file_with_asm_extension() {
    let result = run_asm_cli(&["main.asm", "-o", "out.asm"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["invalid file extension asm for file out.asm. Valid file extensions are [\"bin\"]"]
    );
}

#[test]
fn test_err_source_file_with_bin_extension() {
    let result = run_asm_cli(&["main.bin", "-o", "out.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "invalid file extension bin for file main.bin. Valid file extensions are [\"asm\"]",
            "No source files given"
        ]
    );
}

#[test]
fn test_err_multiple_sources_one_invalid() {
    let result = run_asm_cli(&["valid.asm", "invalid.txt", "-o", "out.bin"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["invalid file extension txt for file invalid.txt. Valid file extensions are [\"asm\"]"]
    );
}

#[test]
fn test_err_both_source_and_output_extensions_invalid() {
    let result = run_asm_cli(&["main.bad", "-o", "out.wrong"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "invalid file extension bad for file main.bad. Valid file extensions are [\"asm\"]",
            "No source files given",
            "invalid file extension wrong for file out.wrong. Valid file extensions are [\"bin\"]"
        ]
    );
}

// ============================================================================
// 6. Subcommands Consuming Unexpected Tokens / Flags
// ============================================================================

#[test]
fn test_err_output_flag_takes_another_flag() {
    let result = run_asm_cli(&["main.asm", "-o", "-S"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["failed to get file_extension for file -S"]
    );
}

#[test]
fn test_err_source_dir_flag_takes_another_flag() {
    let result = run_asm_cli(&["-S", "-o", "main.asm"]);
    assert!(result.is_err());
    assert_eq!(result.errors, vec!["No output file given"]);
}

#[test]
fn test_err_unknown_flag() {
    let result = run_asm_cli(&["-x", "-o", "out.bin", "main.asm"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec!["failed to get file_extension for file -x"]
    );
}

#[test]
fn test_err_help_flag() {
    let result = run_asm_cli(&["--help"]);
    assert!(result.is_err());
    assert_eq!(
        result.errors,
        vec![
            "failed to get file_extension for file --help",
            "No source files given",
            "No output file given"
        ]
    );
}
