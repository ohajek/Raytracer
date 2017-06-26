#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "Random.h"
#include "Hitable.h"


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

static bool refract(const glm::vec3 &v, const glm::vec3 &n, float ni_over_nt, glm::vec3 &refracted) {
	glm::vec3 uv = glm::normalize(v);
	float dt = glm::dot(uv, n);
	float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0) {
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	return false;
}

static float schlick(float cosine, float ref_indx) {
	float r0 = (1 - ref_indx) / (1 + ref_indx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
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

class Dielectric : public Material {
public:
	Dielectric() {};
	Dielectric(float r) : ref_indx(r) {};
	virtual bool scatter(const Ray& r_in, const hit_record& rec, glm::vec3& attenuation, Ray& scattered) const {
		glm::vec3 outward_normal;
		glm::vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = glm::vec3(1.0f, 1.0f, 1.0f); //kills blue color bug
		glm::vec3 refracted;
		float reflect_prob;
		float cosine;
		if (glm::dot(r_in.direction(), rec.normal) > 0) {
			outward_normal = -rec.normal;
			ni_over_nt = ref_indx;
			cosine = ref_indx * glm::dot(r_in.direction(), rec.normal) / glm::length(r_in.direction());
		}
		else {
			outward_normal = rec.normal;
			ni_over_nt = 1.0f / ref_indx;
			cosine = -glm::dot(r_in.direction(), rec.normal) / glm::length(r_in.direction());
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
			reflect_prob = schlick(cosine, ref_indx);
		}
		else {
			scattered = Ray(rec.p, reflected);
			reflect_prob = 1.0f;
		}
		myRand::Xorshift rng(rand());
		if (rng() < reflect_prob) {
			scattered = Ray(rec.p, reflected);
		}
		else {
			scattered = Ray(rec.p, refracted);
		}
		return true;
	}

	float ref_indx;
};

#endif