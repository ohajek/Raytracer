#ifndef SPHERE_H
#define SPHERE_H
#pragma once
#include "Hitable.h"

class Sphere : public Hitable {
public:
	Sphere() {};
	~Sphere() {};
	Sphere(glm::vec3 cen, float r, Material *m_ptr) : center(cen), radius(r), material_ptr(m_ptr) {};
	virtual bool hit(const Ray &r, float t_min, float t_max, hit_record &rec) const;
	
	glm::vec3 center;
	float radius;
	Material *material_ptr;
};

#endif