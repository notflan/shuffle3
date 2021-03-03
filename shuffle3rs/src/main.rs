
#![allow(dead_code)]

#[cfg(feature="deferred-drop")] #[macro_use] extern crate lazy_static;
#[macro_use] extern crate cfg_if;

#[macro_use] mod ext; use ext::*;

#[cfg(feature="deferred-drop")] mod defer_drop;
mod shuffle;
mod arg;

use arg::Mode;

fn main() {
    match arg::parse_args()
    {
	Ok(Mode::Help) => arg::usage(),
	Ok(Mode::ShuffleInPlace(file)) => {
	    todo!()  
	},
	Ok(Mode::UnshuffleInPlace(file)) => {
	    todo!()
	},
	Err(err) => {
	    eprintln!("Error: {}", err);
	    eprintln!("\nTry passing `-h`");
	    std::process::exit(1)
	}
    }
}

#[cfg(test)] mod test;
