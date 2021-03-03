

/// The executable program name
pub fn program_name() -> &'static str
{
    lazy_static! {
	static ref NAME: String = std::env::args().next().unwrap();
    }
    &NAME[..]
}

#[cfg(feature="splash")]
#[inline] fn splash()
{
    println!("shuffle3rs (v{}) - improved 3 pass binary shuffler (Rust ver)", env!("CARGO_PKG_VERSION"));
    println!(" written by {} with <3", env!("CARGO_PKG_AUTHORS"));
    println!(" licensed with GPL v3.0 or later\n");
}

/// Print usage message
pub fn usage()
{
    #[cfg(feature="splash")] splash();

    println!("Usage: {} -s <file>", program_name());
    println!("Usage: {} -u <file>", program_name());
    println!("\nOPTIONS:");
    println!("  -s\tShuffle file in place");
    println!("  -u\tUnshuffle file in place");
}

/// Print usage message and then exit with error code `2`.
#[inline] pub fn help() -> !
{
    usage();

    std::process::exit(2)
}
