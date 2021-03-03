//! Actual processing
use super::*;
use std::path::{Path, PathBuf};
use std::io;

/// What kind of operation are we doing?
#[derive(Debug, Clone, PartialEq, Eq, Hash, Copy)]
pub enum ModeKind
{
    Shuffle,
    Unshuffle
}

pub fn shuffle_file(file: impl AsRef<Path>) -> io::Result<()>
{
    todo!()	
}

pub fn unshuffle_file(file: impl AsRef<Path>) -> io::Result<()>
{
    todo!()
}

/// Process these files in place with this mode
pub fn process_files_ip<I>(files: I, mode: ModeKind) -> io::Result<()>
where I: IntoIterator<Item = PathBuf>
{

    todo!()
}
