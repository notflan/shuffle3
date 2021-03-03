
#![allow(dead_code)]

#[cfg(feature="deferred-drop")] #[macro_use] extern crate lazy_static;
#[macro_use] extern crate cfg_if;

#[macro_use] mod ext; use ext::*;

#[cfg(feature="deferred-drop")] mod defer_drop;
mod shuffle;



fn main() {
    println!("Hello, world!");
}

#[cfg(test)] mod test;
