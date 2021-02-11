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
