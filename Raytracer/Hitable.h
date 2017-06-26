#ifndef HITABLE_H
#define HITABLE_h
#pragma once

#include "Ray.h"
#include "Material.h"

class Material;

struct hit_record {
	float t;
	glm::vec3 p;
	glm::vec3 normal;
	Material *mat_ptr;
};

class Hitable {
public:
	virtual bool hit(const Ray &r, float t_min, float t_max, hit_record &rec) const = 0;
};

static glm::vec3 random_in_unit_sphere() {
	glm::vec3 p;
	do {
		myRand::Xorshift rng(rand());
		p = 2.0f * glm::vec3(rng(), rng(), rng()) - glm::vec3(1, 1, 1);
	} while (glm::pow(glm::length(p), 2)/*p.squared_length()*/ >= 1.0f);
	return p;
}

static glm::vec3 reflect(const glm::vec3 &v, const glm::vec3 &n) {
	return v - 2 * glm::dot(v, n)*n;
}

class Material {
public:
	virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec3& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material {
public:
	Lambertian() {};
	Lambertian(const glm::vec3 &a) : albedo(a) {}
	virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec3& attenuation, Ray& scattered) const {
		glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = Ray(rec.p, target - rec.p);
		attenuation = albedo;
		return true;
	}

	glm::vec3 albedo;
};

class Metal : public Material {
public:
	Metal() {};
	Metal(const glm::vec3 &a, float f) : albedo(a) {
		if (f < 1) fuzz = f; else fuzz = 1;
	}
	virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec3& attenuation, Ray& scattered) const {
		glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
		scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (glm::dot(scattered.direction(), rec.normal) > 0);
	}

	glm::vec3 albedo;
	float fuzz;
};

#endif