//! Actual processing
use super::*;
use std::path::{Path, PathBuf};
use std::io;
use std::fs;

use memmap::MmapMut;

/// What kind of operation are we doing?
#[derive(Debug, Clone, PartialEq, Eq, Hash, Copy)]
pub enum ModeKind
{
    Shuffle,
    Unshuffle
}

fn load_file_ip(file: impl AsRef<Path>) -> io::Result<memmap::MmapMut>
{
    let file = fs::OpenOptions::new()
        .read(true)
        .write(true)
        .open(file)?;
    Ok(unsafe {
	MmapMut::map_mut(&file)?
    })
}

pub fn shuffle_file_ip(file: impl AsRef<Path>) -> io::Result<()>
{
    let mut file = load_file_ip(file)?;
    todo!("shuffling");
    file.flush()?;

    Ok(())
}

pub fn unshuffle_file_ip(file: impl AsRef<Path>) -> io::Result<()>
{
    let mut file = load_file_ip(file)?;
    todo!("unshuffling");
    file.flush()?;

    Ok(())
}

/// Process these files in place with this mode
pub fn process_files_ip<I>(files: I, mode: ModeKind) -> io::Result<()>
where I: IntoIterator<Item = PathBuf>
{
    cfg_if! {
	if #[cfg(feature="threads")] {
	    let handles: Vec<_> = files.into_iter().map(|file| {
		std::thread::spawn(move || {
		    let res = match mode {
			ModeKind::Shuffle => shuffle_file_ip(&file),
			ModeKind::Unshuffle => unshuffle_file_ip(&file),
		    };
		    (res, file)
		})
	    }).collect();

	    for h in handles
	    {
		let (res, file) = h.join().expect("Worker thread panicked"); // TODO: How do we communicate which thread failed here? Should we even try?
		
		res.map_err(move |x| {
		    eprintln!("Failed to process {:?}", file);
		    x
		})?;
	    }
	    Ok(())
	} else {
	    for file in files.into_iter() {
		let res = match mode {
		    ModeKind::Shuffle => shuffle_file_ip(&file),
		    ModeKind::Unshuffle => unshuffle_file_ip(&file),
		};		
		res.map_err(move |x| {
		    eprintln!("Failed to process {:?}", file);
		    x
		})?;
	    }
	    Ok(())
	}
    }
}
