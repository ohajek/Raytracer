#include "Sphere.h"

bool Sphere::hit(const Ray &r, float t_min, float t_max, hit_record &rec) const {
	glm::vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b * b - a * c; //4
	if (discriminant > 0) {
		float temp = (-b - sqrt(b*b - a*c)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = material_ptr;
			return true;
		}
		temp = (-b + sqrt(b*b - a*c)) / a;
		if (temp < t_max && temp > t_min) {
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = material_ptr;
			return true;
		}
	}
	return false;
}