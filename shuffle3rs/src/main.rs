
#![allow(dead_code)]

#[cfg(feature="deferred-drop")] #[macro_use] extern crate lazy_static;
#[macro_use] extern crate cfg_if;

#[macro_use] mod ext; use ext::*;

#[cfg(feature="deferred-drop")] mod defer_drop;
mod shuffle;
mod arg;
mod proc;

use arg::Mode;

/// Handle a fatal error
///
/// # Method
/// On `Err`, print the error message and then exit with error code provided
fn handle_err_fatal<T, E>(op: Result<T, E>, errcd: i32) -> T
    where E: std::error::Error
{
    match op
    {
	Ok(v) => v,
	Err(err) => {
	    eprintln!("Fatal error: {}", err);
	    std::process::exit(errcd)
	}
    }
}

fn main() {
    match arg::parse_args()
    {
	Ok(Mode::Help) => arg::usage(),
	Ok(Mode::ShuffleInPlace(file)) => {
	    handle_err_fatal(proc::process_files_ip(file, proc::ModeKind::Shuffle), -1);
	},
	Ok(Mode::UnshuffleInPlace(file)) => {
	    handle_err_fatal(proc::process_files_ip(file, proc::ModeKind::Unshuffle), -1);
	},
	Err(err) => {
	    eprintln!("Error: {}", err);
	    eprintln!("\nTry passing `-h`");
	    std::process::exit(1)
	}
    }
}

#[cfg(test)] mod test;
