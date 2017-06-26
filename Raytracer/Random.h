#ifndef RANDOM_H
#define RANDOM_H

namespace myRand {

	class Xorshift {
	public:
		//Stav RNG
		unsigned int x;
		const int fmask = (1 << 23) - 1;
		Xorshift(const unsigned int seed) { x = seed; }
		int next() {
			x ^= x >> 6;
			x ^= x << 17;
			x ^= x >> 9;
			return int(x);
		}
		float operator()(void) {
			union {
				float f;
				int i;
			} u;
			u.i = (next() & fmask) | 0x3f800000;
			return u.f - 1.f;
		}
	};
}

#endif