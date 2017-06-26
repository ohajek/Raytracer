#include <iostream>
#include <fstream>
#include <limits>
#include <float.h>
#include <random>

#include "Sphere.h"
#include "HitableList.h"
#include "Camera.h"
#include "Random.h"


bool openIO(std::fstream& file, const char* filename) {
	file.open(filename); // Try to open for I/O
	if (!file) {
		file.clear();
		file.open(filename, std::ios::out); // Create using out
		if (!file) {
			return false;
		}
		file.close();
		file.open(filename); // Open for I/O
		if (!file) {
			return false;
		}
	}
	return true;
}

glm::vec3 random_in_unit_sphere() {
	glm::vec3 p;
	do {
		myRand::Xorshift rng(rand());
		p = 2.0f * glm::vec3(rng(), rng(), rng()) - glm::vec3(1, 1, 1);
	} while (glm::pow(glm::length(p), 2)/*p.squared_length()*/ >= 1.0f);
	return p;
}

glm::vec3 color(const Ray& r, Hitable *world) {
	hit_record rec;
	if (world->hit(r, 0.0f, FLT_MAX, rec)) {
		//return 0.5f * glm::vec3(rec.normal.x + 1, rec.normal.y + 1, rec.normal.z + 1);
		glm::vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5f * color(Ray(rec.p, target - rec.p), world);
	}
	else {
		glm::vec3 unit_direction = glm::normalize(r.direction());/*unit_vector(r.direction())*/;
		float t = 0.5f * (unit_direction.y + 1.0f);
		return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
	}
}

int main() {
	int nx = 200;
	int ny = 100;
	int ns = 10;
	unsigned int numPixels = nx * ny;
	unsigned int *framebuffer;
	framebuffer = new unsigned int[numPixels];
	for (auto i = 0; i < numPixels; i++) {
		framebuffer[i] = 0;
	}
	std::fstream file;
	if (!openIO(file, "render.ppm")) {
		std::cerr << "Error while opening file";
		return 1;
	}

	file << "P3\n" << nx << " " << ny << "\n255\n";
	Hitable *list[2];
	list[0] = new Sphere(glm::vec3(0, 0, -1), 0.5f);
	list[1] = new Sphere(glm::vec3(0, -100.5f, -1), 100);
	Hitable *world = new HitableList(list, 2);
	Camera cam;
	glm::vec3 col(0, 0, 0);
	//TODO: sprav to#pragma omp parallel for private(col)
	for (int j = ny-1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			col = glm::vec3(0.0f);
			for (int s = 0; s < ns; s++) {
				myRand::Xorshift rng(rand());
				float u = float(i + rng()) / float(nx);
				float v = float(j + rng()) / float(ny);
				Ray r = cam.get_ray(u, v);
				glm::vec3 p = r.point_at_parameter(2.0f);
				col += color(r, world);
			}
			col /= float(ns);
			col = glm::vec3(sqrt(col.r), sqrt(col.g), sqrt(col.b));
			int ir = int(255.99 * col.r);
			int ig = int(255.99 * col.g);
			int ib = int(255.99 * col.b);
			file << ir << " " << ig << " " << ib << "\n";
		}
	}
	file.close();
	return 0;
}