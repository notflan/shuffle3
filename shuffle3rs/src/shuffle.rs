use rand::Rng;
use std::mem::swap;

/// Get a random element in this slice.
pub fn element_in<'a, T, R: Rng + ?Sized>(slice: &'a (impl AsRef<[T]> + ?Sized), with: &mut R) -> &'a T
{
    let slice = slice.as_ref();
    &slice[with.gen_range(0..slice.len())]
}

/// Get a random element in this slice.
pub fn element_in_mut<'a, T, R: Rng + ?Sized>(slice: &'a mut (impl AsMut<[T]> + ?Sized), with: &mut R) -> &'a mut T
{
    inside(slice.as_mut(), with)
}


#[inline(always)] fn inside<'a, T, R: Rng + ?Sized>(slice: &'a mut [T], with: &mut R) -> &'a mut T
{
    &mut slice[with.gen_range(0..slice.len())]
}

fn shuffle_slice<T, R: Rng + ?Sized>(slice: &mut [T], with: &mut R)
{
    match slice
    {
	&mut [] | &mut [_] => {},
	&mut [ref mut inner @ .. , ref mut b] => {
	    swap(b, inside(inner, with));
	    shuffle(inner, with);
	},
    }
}

fn unshuffle_slice<T, R: Rng + ?Sized>(slice: &mut [T], with: &mut R)
{
    let indecies: Vec<_> = (1..slice.len()).map(|idx| with.gen_range(0..idx)).collect(); 

    todo!();

    drop!(indecies);
}
/// Shuffle this slice with this `Rng`.
#[inline(always)] pub fn shuffle<T, R: Rng + ?Sized>(mut slice: impl AsMut<[T]>, with: &mut R)
{
    shuffle_slice(slice.as_mut(), with)
}
