
#![allow(dead_code)]

#[cfg(feature="deferred-drop")] #[macro_use] extern crate lazy_static;
#[macro_use] extern crate cfg_if;

#[macro_use] mod ext; use ext::*;

#[cfg(feature="deferred-drop")] mod defer_drop;
mod shuffle;



fn main() {
    println!("Hello, world!");

    #[cfg(test)] {
	let mut values: Vec<_> = (0..100000i32).collect();
	let expected = values.clone();
	
	let mut rng = {
	    use rand::prelude::*;
	    rand_chacha::ChaChaRng::from_seed([0xfa; 32])
	};
	let mut nrng = rng.clone();
	
	println!("Start (first 10): {:?}", &values[..10]);

	values.shuffle(&mut rng);

	println!("Shuffled (first 10): {:?}", &values[..10]);

	values.unshuffle(&mut nrng);

	println!("Unshuffled (first 10): {:?}", &values[..10]);

	assert_eq!(&values[..], &expected[..]);
    }
}

#[cfg(test)] mod test;
