#pragma once

#include "Hitable.h"

class HitableList : public Hitable {
public:
	HitableList() {};
	HitableList(Hitable **l, int n) : list(l), list_size(n) {};
	virtual bool hit(const Ray &r, float t_min, float t_max, hit_record &rec) const;

	Hitable **list;
	int list_size;
};