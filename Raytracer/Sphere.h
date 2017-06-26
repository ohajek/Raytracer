#pragma once
#include "Hitable.h"
class Sphere : public Hitable {
public:
	Sphere() {};
	~Sphere() {};
	Sphere(glm::vec3 cen, float r) : center(cen), radius(r) {};
	virtual bool hit(const Ray &r, float t_min, float t_max, hit_record &rec) const;
	
	glm::vec3 center;
	float radius;
};
