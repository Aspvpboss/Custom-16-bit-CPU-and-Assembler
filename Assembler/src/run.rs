use crate::error_handling::AsmError;
use crate::asm::lexical_analysis;

use peak_alloc::PeakAlloc;

#[global_allocator]
static PEAK_ALLOC: PeakAlloc = PeakAlloc;


pub fn run() -> Result<(), Vec<AsmError>>{
    
    
    let lexed_tokens = lexical_analysis::lexical_analysis()?;


    // for lexed_token in &lexed_tokens{
    //     println!("{}", lexed_token);
    // }

    println!("current heap KB: {}", PEAK_ALLOC.current_usage_as_kb());

    println!("peak heap KB: {}", PEAK_ALLOC.peak_usage_as_kb());

    Ok(())
}

