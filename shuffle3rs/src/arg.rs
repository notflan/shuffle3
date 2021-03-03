use super::*;

use std::{fmt, error};
use std::path::PathBuf;

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

pub enum Mode
{
    Help,
    ShuffleInPlace(MaybeVec<PathBuf>),
    UnshuffleInPlace(MaybeVec<PathBuf>),
}

/// Parse the `argv` of this process.
#[inline] pub fn parse_args() -> Result<Mode, Error>
{
    parse(std::env::args().skip(1))
}

fn parse<I: IntoIterator<Item = String>>(args: I) -> Result<Mode, Error>
{
    #[inline] fn validate_file(p: impl Into<PathBuf>) -> Result<PathBuf, Error>
    {
	let p = p.into();
	if p.is_file()
	{
	    Ok(p)
	} else {
	    Err(Error::FileNotFound(p))
	}
    }
    
    let mut args = args.into_iter();

    let farg = args.next();
    match farg
    {
	Some(yes) => {
	    match yes.trim()
	    {		
		"-h" => Ok(Mode::Help),
		"-s" => {
		    Ok(Mode::ShuffleInPlace(
			args
			    .map(|file| validate_file(file)).collect::<Result<MaybeVec<_>, Error>>()
			    .and_then(|v| if v.len() == 0 { Err(Error::InvalidUseArg("-s"))} else {Ok(v)})?))
		},
		"-u" => {
		    Ok(Mode::UnshuffleInPlace(
			args
			    .map(|file| validate_file(file)).collect::<Result<MaybeVec<_>, Error>>()
			    .and_then(|v| if v.len() == 0 { Err(Error::InvalidUseArg("-u"))} else {Ok(v)})?))

		},
		_ => Err(Error::UnknownOpt(yes))
	    }
	}
	None => Err(Error::NoOpt),
    }
}

/// Arg parsing error
#[derive(Debug)]
#[non_exhaustive]
pub enum Error
{
    InvalidUseArg(&'static str),
    NoOpt,
    
    UnknownOpt(String),
    FileNotFound(PathBuf),
}

impl error::Error for Error{}
impl fmt::Display for Error
{
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result
    {
	match self {
	    Self::NoOpt => write!(f, "No options specified"),
	    Self::UnknownOpt(opt) => write!(f, "Unknown option {:?}", opt),
	    Self::FileNotFound(file) => write!(f, "File {:?} not found", file),
	    Self::InvalidUseArg(string) => write!(f, "Argument {:?} expects a parameter", string),
	}
    }
}
