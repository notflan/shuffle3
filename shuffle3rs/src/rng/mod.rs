//! Contains the RNGs used 
use super::*;
use std::{ptr, slice, mem::{self, MaybeUninit}};
use std::ops::Drop;

/// A tuple packed into a type that can be used for SeedableRng::SEED.
#[derive(Debug)]
pub struct PackedTupleSeed<T>([MaybeUninit<T>; 2]);

impl<T: Eq> Eq for PackedTupleSeed<T>{}
impl<T: PartialEq> PartialEq for PackedTupleSeed<T>
{
    fn eq(&self, other: &Self) -> bool
    {
	self.first() == other.first() &&
	    self.second() == other.second()
    }
}

impl<T> PackedTupleSeed<T>
{
    pub const SIZE_BYTES: usize = mem::size_of::<T>() * 2;
    pub fn as_mut_array(&mut self) -> &mut [T; 2]
    {
	unsafe {&mut *(&mut self.0 as *mut [_; 2] as *mut [T; 2])}
    }
    pub fn as_array(&self) -> &[T; 2]
    {
	unsafe {& *(&self.0 as *const [_; 2] as *const [T; 2])}
    }
    pub fn second_mut(&mut self) -> &mut T
    {
	unsafe {&mut *self.0[1].as_mut_ptr()}
    }
    pub fn first_mut(&mut self) -> &mut T
    {
	unsafe {&mut *self.0[0].as_mut_ptr()}
    }
    pub fn second(&self) -> &T
    {
	unsafe {& *self.0[1].as_ptr()}
    }
    pub fn first(&self) -> &T
    {
	unsafe {& *self.0[0].as_ptr()}
    }

    #[inline] pub fn new(a: T, b: T) -> Self {
	Self ([
	    MaybeUninit::new(a),
	    MaybeUninit::new(b),
	])
    }

    #[inline] pub fn into_tuple(self) -> (T, T)
    {
	let ab = unsafe {
	    (self.0[0].as_ptr().read(),
	    self.0[1].as_ptr().read())
	};
	mem::forget(self);
	ab
    }

    #[inline] pub fn into_second(self) -> T
    {
	self.into_tuple().1
    }
    #[inline] pub fn into_first(self) -> T
    {
	self.into_tuple().0
    }
}

impl<T> From<(T, T)> for PackedTupleSeed<T>
{
    fn from((a,b): (T, T)) -> Self
    {
	Self::new(a, b)
    }
}

impl<T> From<PackedTupleSeed<T>> for (T, T)
{
    fn from(from: PackedTupleSeed<T>) -> Self
    {
	from.into_tuple()
    }
}

impl<T: Clone> Clone for PackedTupleSeed<T>
{
    fn clone(&self) -> Self {
	Self([
	    MaybeUninit::new(self.first().clone()),
	    MaybeUninit::new(self.second().clone()),
	])
    }
}

impl<T: Default> Default for PackedTupleSeed<T>
{
    #[inline]
    fn default() -> Self
    {
	Self([
	    MaybeUninit::new(Default::default()),
	    MaybeUninit::new(Default::default()),
	])
    }
}

impl<T> AsMut<[u8]> for PackedTupleSeed<T>
{
    fn as_mut(&mut self) -> &mut [u8]
    {
	unsafe {
	    slice::from_raw_parts_mut(self as *mut Self as *mut u8, mem::size_of::<Self>())
	}
    }
}
impl<T> AsRef<[u8]> for PackedTupleSeed<T>
{
    fn as_ref(&self) -> &[u8]
    {
	unsafe {
	    slice::from_raw_parts(self as *const Self as *const u8, mem::size_of::<Self>())
	}
    }
}

impl<T> Drop for PackedTupleSeed<T>
{
    fn drop(&mut self) {
	if mem::needs_drop::<T>() {
	    unsafe {
		ptr::drop_in_place(self.0[0].as_mut_ptr());
		ptr::drop_in_place(self.0[1].as_mut_ptr());
	    }
	}
    }
}
