use super::*;

#[test]
pub fn shuffle_then_back()
{
    let mut values: Vec<_> = (0..10i32).collect();
    let expected = values.clone();
    
    let mut rng = {
	use rand::prelude::*;
	rand_chacha::ChaChaRng::from_seed([12; 32])
    };
    let mut nrng = rng.clone();
    
    println!("Start: {:?}", values);

    values.shuffle(&mut rng);

    println!("Shuffled: {:?}", values);

    values.unshuffle(&mut nrng);

    println!("Unshuffled: {:?}", values);

    assert_eq!(&values[..], &expected[..]);
}


#[test]
pub fn shuffle_then_back_large()
{
    let mut values: Vec<_> = (0..10000i32).collect();
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
