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
    //XXX: Without optimisations this recursion causes stack overflow. Cannot work without tail call optimisation
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
    let indecies: Vec<_> = (1..slice.len()).rev().map(|x| with.gen_range(0..x)).collect();
    
    for (i, &idx) in (1..slice.len()).zip(indecies.iter().rev())
    {
	slice.swap(i, idx);
    }

    drop!(indecies);
}
/// Shuffle this slice with this `Rng`.
#[inline(always)] pub fn shuffle<T, R: Rng + ?Sized>(mut slice: impl AsMut<[T]>, with: &mut R)
{
    shuffle_slice(slice.as_mut(), with)
}

#[inline(always)] pub fn unshuffle<T, R: Rng + ?Sized>(mut slice: impl AsMut<[T]>, with: &mut R)
{
    unshuffle_slice(slice.as_mut(), with)
}
