use super::*;

use std::{
    thread,
    time::Duration,
    sync::mpsc,
    any::Any,
    marker::{Send, Sync},
};

pub type Defer = Box<dyn Any + Send + 'static>;

/// A deferred dropping handle.
#[derive(Debug, Clone)]
#[repr(transparent)]
pub struct DeferredDropper(mpsc::Sender<Defer>);

impl DeferredDropper
{
    /// Drop a Boxed item on the background thread
    ///
    /// # Panics
    /// If the background thread has panicked.
    #[inline] pub fn drop_boxed<T: Any + Send + 'static>(&self, item: Box<T>)
    {
	self.0.send(item).unwrap();
    }
    /// Drop an item on the background thread
    ///
    /// # Panics
    /// If the background thread has panicked.
    #[inline(always)] pub fn drop<T: Any +Send+ 'static>(&self, item: T)
    {
	self.drop_boxed(Box::new(item))
    }

    /// Send this deferring drop
    ///
    /// # Panics
    /// If the background thread has panicked.
    #[inline(always)] pub fn send(&self, item: impl Into<Defer>)
    {
	self.0.send(item.into()).unwrap()
    }
}

/// Subscribe to the deferred dropper. Usually avoid doing this, and just use the `drop!` macro, or use the thread-local static dropper reference `HANDLE` directly.
fn defer_drop_sub() -> DeferredDropper
{
    use std::sync::Mutex; // Can we get rid of this somehow? I don't think we can. Either we mutex lock here, or we mutex lock on every (I think every, should look into that..) send. I think this is preferrable.
    #[repr(transparent)]
    struct Shim(Mutex<mpsc::Sender<Defer>>);

    unsafe impl Sync for Shim{}
    
    lazy_static! {
	static ref TX: Shim = {
	    let (tx, rx) = mpsc::channel();
	    thread::spawn(move || {
		for val in rx.into_iter()
		    .lag(Duration::from_millis(10))
		{
		    //let _ = thread::spawn(move || drop(val)).join(); // To catch panic?
		    drop(val); // What if this panics?
		}
	    });
	    Shim(Mutex::new(tx))
	};
    }

    DeferredDropper(TX.0.lock().unwrap().clone())
}

thread_local! {
    pub static HANDLE: DeferredDropper = defer_drop_sub();
}

#[cfg(test)]
mod tests
{
    #[test]
    fn mac()
    {
	use crate::*;
	let sub = super::defer_drop_sub();

	let large_vec = vec![String::from("hello world"); 1000];

	
	drop!(in sub; large_vec.clone());
	drop!(large_vec);
	drop!(box Box::new("hello world?"));
	drop!(in sub; box Box::new("hello world?"));
    }
    
    #[test]
    fn dropping_larges()
    {
	for joiner in std::iter::repeat_with(|| {
	    let large_vec = vec![String::from("hello world"); 1000];
	    let h = {
		let mut large_vec = large_vec.clone();
		std::thread::spawn(move || {
		    large_vec.sort();
		    drop!(large_vec);
		})
	    };
	    drop!(large_vec);
	    h
	}).take(1000)
	{
	    joiner.join().unwrap();
	}
	std::thread::sleep(std::time::Duration::from_millis(500));
    }
    #[test]
    fn dropping_vec()
    {
	let sub = super::defer_drop_sub();

	let large_vec = vec![String::from("hello world"); 1000];


	sub.drop(large_vec.clone());
	sub.drop_boxed(Box::new(large_vec)) //we can't just send boxed slice because MUH SIZED???
	//FUCK THIS! J)QI EJOAIJAOIW
	/*
	    unsafe {
	    let raw = Box::into_raw(large_vec.into_boxed_slice());
	    sub.send(Box::from_raw(raw as *mut (dyn std::any::Any + Send + 'static))).unwrap();
    }*/
    }
    #[test]
    fn clone_shim()
    {
	for joiner in std::iter::repeat_with(|| {
	    std::thread::spawn(move || {
		let mut subs = Vec::new();
		for _ in 0..100 {
		    subs.push(super::defer_drop_sub());
		}
	    })
	}).take(1000)
	{
	    joiner.join().unwrap();
	}
    }
}
