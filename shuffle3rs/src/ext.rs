use std::time::Duration;
use std::iter::{FusedIterator, DoubleEndedIterator};

use rand::Rng;
use crate::shuffle;

pub trait SliceElementExt<T>
{
    fn random_element<R: Rng + ?Sized>(&self, rng: &mut R) -> &T;
    fn random_element_mut<R: Rng + ?Sized>(&mut self, rng: &mut R) -> &mut T;
}

pub trait ShuffleExt<T>
{
    fn shuffle<R: Rng + ?Sized>(&mut self, rng: &mut R);
    fn unshuffle<R: Rng + ?Sized>(&mut self, rng: &mut R);
}

impl<T> SliceElementExt<T> for [T]
{
    #[inline] fn random_element<R: Rng + ?Sized>(&self, rng: &mut R) -> &T {
	shuffle::element_in(self, rng)
    }
    #[inline] fn random_element_mut<R: Rng + ?Sized>(&mut self, rng: &mut R) -> &mut T {
	shuffle::element_in_mut(self, rng)
    }
}

impl<T> ShuffleExt<T> for [T]
{
    #[inline] fn shuffle<R: Rng + ?Sized>(&mut self, rng: &mut R) {
	shuffle::shuffle(self, rng)
    }
    #[inline] fn unshuffle<R: Rng + ?Sized>(&mut self, rng: &mut R) {
	todo!()
    }
}

/// A lagged iterator
#[derive(Debug, Clone)]
pub struct Lag<I: ?Sized, T>(Duration, I)
where I: Iterator<Item=T>;


impl<I: ?Sized, T>  Lag<I, T>
where I: Iterator<Item=T>
{
    /// Set the lag duration
    #[inline] pub fn set_duration(&mut self, dur: Duration)
    {
	self.0 = dur;
    }
    /// Get the lag duration
    #[inline] pub fn duration(&self) -> Duration
    {
	self.0
    }
}
impl<I, T>  Lag<I, T>
where I: Iterator<Item=T>
{
    /// Consume into the inner iterator
    #[inline] pub fn into_inner(self) -> I
    {
	self.1
    }
}

pub trait LagIterExt: Iterator
{
    fn lag(self, dur: Duration) -> Lag<Self, Self::Item>;
}

impl<I> LagIterExt for I
where I: Iterator
{
    #[inline] fn lag(self, dur: Duration) -> Lag<Self, Self::Item>
    {
	Lag(dur, self)
    }
}

impl<I: ?Sized, T> Iterator for Lag<I, T>
where I: Iterator<Item=T>
{
    type Item = T;
    fn next(&mut self) -> Option<Self::Item>
    {
	std::thread::sleep(self.0);
	self.1.next()
    }
    fn size_hint(&self) -> (usize, Option<usize>) {
	self.1.size_hint()
    }
}

impl<I: ?Sized, T> FusedIterator for Lag<I, T>
where I: Iterator<Item=T> + FusedIterator{}
impl<I: ?Sized, T> ExactSizeIterator for Lag<I, T>
where I: Iterator<Item=T> + ExactSizeIterator{}
impl<I: ?Sized, T> DoubleEndedIterator for Lag<I, T>
where I: Iterator<Item=T> + DoubleEndedIterator
{
    fn next_back(&mut self) -> Option<Self::Item>
    {
	std::thread::sleep(self.0);
	self.1.next()
    }
}

cfg_if!{

    if #[cfg(feature="deferred-drop")] {
	/// Drop this item on another thread
	#[macro_export] macro_rules! drop {
	    (in $sub:ident; box $val:expr) => {
		$sub.drop_boxed($val)
	    };
	    (in $sub:ident; $val:expr) => {
		$sub.drop($val)
	    };
	    (box $val:expr) => {
		{		
		    $crate::defer_drop::HANDLE.with(move |sub| {
			sub.drop($val)	    
		    });
		}
	    };
	    ($val:expr) => {
		{
		    $crate::defer_drop::HANDLE.with(move |sub| {
			sub.drop($val)	    
		    });
		}
	    };

	}
    } else {
	/// Drop this item on another thread
	#[macro_export] macro_rules! drop {
	    (box $val:expr) => {
		{
		    drop($val)
		}
	    };
	    ($val:expr) => {
		{
		    drop($val)
		}
	    };

	}
    }
}
