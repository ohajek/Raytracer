#include <iostream>
#include <fstream>
#include <limits>
#include <float.h>
#include <random>

#include "Sphere.h"
#include "HitableList.h"
#include "Camera.h"
#include "Random.h"
#include "Material.h"

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

glm::vec3 color(const Ray& r, Hitable *world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001f, FLT_MAX, rec)) {
		Ray scattered;
		glm::vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		}
		else {
			return glm::vec3(0.0f);
		}
	}
	else {
		glm::vec3 unit_direction = glm::normalize(r.direction());
		float t = 0.5f * (unit_direction.y + 1.0f);
		return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
	}
}

int main() {
	int nx = 800;
	int ny = 400;
	int ns = 1;
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
	Hitable *list[5];
	list[0] = new Sphere(glm::vec3(0, 0, -1), 0.5f, new Lambertian(glm::vec3(0.8f, 0.3f, 0.3f)));
	list[1] = new Sphere(glm::vec3(0, -100.5f, -1), 100,  new Lambertian(glm::vec3(0.8f, 0.8f, 0.0f)));
	list[2] = new Sphere(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f,  new Metal(glm::vec3(0.8f, 0.6f, 0.2f), 0.3f));
	list[3] = new Sphere(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f,  new Dielectric(1.5f));
	list[4] = new Sphere(glm::vec3(-1.0f, 0.0f, -1.0f), -0.45f,  new Dielectric(1.5f));
	Hitable *world = new HitableList(list, 5);
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
				//glm::vec3 p = r.point_at_parameter(2.0f);
				col += color(r, world, 0);
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