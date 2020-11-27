#include <rng_impl.h>
#include <math.h>
#include <string.h>

_Static_assert(sizeof(double)==8, "Double must be 64-bits.");

double vec[2];

static void _seed(const void* from)
{
	memcpy(vec, from, sizeof(double)*2);

}

double fract(double x)
{
	return x - floor(x);
}

double dot(double *v, double *u, int n)
{
	register double res = 0;
	for(register int i=0;i<n;i++)
	{
		res += v[i]*u[i];
	}
	return res;
}

static double sample_double(double vec[2])
{
	static double vec2[2] = { 12.9898, 78.223 };
	double res = fract(sin(dot(vec, vec2, 2)) * 43758.5453);
	return res;
}
static void next(double r)
{
	float v1 = vec[0];
	float v2 = vec[1];

	double nvec[2];

	nvec[0] = r;
	nvec[1] = v2;
	
	vec[0] = sample_double(nvec);

	nvec[1] = v1;
	vec[1] = sample_double(nvec);
}



static double _sample()
{
	double res = sample_double(vec);
	next(res);
	return res;
}

RNG_IMPL_DEFINITION(frng)
{
	struct rng_algo rng;

	memset(&rng,0,sizeof(rng));

	rng.seed = &_seed;
	rng.sample = &_sample;

	return rng_new_impl(&rng);
}
