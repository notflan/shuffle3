#ifndef _RNG_H
#define _RNG_H

typedef struct rng_algo *RNG; //RNG algorithm reference

#define rng_reinterpret(rng, value, type) (*((type*)rng_next(rng, &value, sizeof(type))))
#define rng_reinterpret_new(rng, value, type) (*((type*)rng_next(rng, malloc(sizeof(type)), sizeof(type))))
#define rng_reinterpret_stackalloc(rng, value, type) rng_reinterpret(rng, alloca(sizeof(type)), type)

#define RNG_LEXBIND(rng, type, name) type name = rng_reinterpret(rng, name, type)

double rng_next_double(RNG algo);
int rng_next_int_bounded(RNG algo, int min, int max);
int rng_next_int(RNG algo, int max);
void* rng_next(RNG algo, void* data, int len);
int rng_chance(RNG algo, double d);
void rng_free(RNG algo);
void rng_seed(RNG algo, void* seed);
RNG rng_new(RNG (*instantiate)(void));

#define RNG_IMPL_DEFINITION(name) RNG __rng_impl_ ## name(void)
#define RNG_ALGO(name) &__rng_impl_ ## name
#define RNG_NEW(name) rng_new(RNG_ALGO(name))

#endif /* _RNG_H */
