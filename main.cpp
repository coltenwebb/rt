#include "rtweekend.h"

#include "sphere.h"
#include "hittable_list.h"
#include "color.h"
#include "camera.h"
#include "material.h"

#include <iostream>

color ray_color(const ray& r, const hittable& world, int depth) {
  if (depth <= 0) {
    return color(0,0,0);
  }

  hit_record rec;

  if (world.hit(r, 0.001, infinity, rec)) {
    ray scattered;
    color attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * ray_color(scattered, world, depth-1);
    }
    return color(0,0,0);
    //point3 target = rec.p + rec.normal + random_in_hemisphere(rec.normal);
    //point3 target = rec.p + rec.normal + random_unit_vector();
    //return .5*ray_color(ray(rec.p, target-rec.p), world, depth-1);
  }

  // clouwds
  vec3 unit_direction = unit_vector(r.direction());
  // -1 <= unit_direction.y() < 1, but actually more restricted 
  // since its normalized
  double t = .5*(unit_direction.y() + 1.0);
  // lerp blue at top, white at bottom
  return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

int main() {
  // image
  const auto aspect_ratio = 16.0 / 9.0;
  const int image_width = 1000;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 400;
  const int max_depth = 50;

  // world
  hittable_list world;
  world.add(make_shared<sphere>(point3(0,0,-1),.49, make_shared<dielectric>(1.9)));
  world.add(make_shared<sphere>(point3(0,0,-1),-.47, make_shared<dielectric>(1.9)));

  world.add(make_shared<sphere>(point3(0,-100,-1),99.5, make_shared<lambertian>(color(.2,.7,.2))));
  world.add(make_shared<sphere>(point3(-1,-.2,-1),.3, make_shared<metal>(color(.9,1,.9))));
  world.add(make_shared<sphere>(point3(1,-.2,-1),.3, make_shared<metal>(color(.5,.5,.9), .1)));
  world.add(make_shared<sphere>(point3(1,-.2,-2),.3, make_shared<metal>(color(.2,.5,.9), .8)));
  world.add(make_shared<sphere>(point3(-1,-.2,0),.3, make_shared<lambertian>(color(.9,.5,.3))));


  // camera
  point3 lookfrom(3,1,3);
  point3 lookat(0,0,-1);
  vec3 vup(0,1,0);
  auto dist_to_focus = (lookfrom-lookat).length();
  auto aperture = 0.07;

  camera cam(lookfrom, lookat, vup, 25, aspect_ratio, aperture, dist_to_focus);

  std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

  for (int j = image_height-1; j > -1; j--) {
    std::cerr << "\rScanlines remaining: " << j << std::flush;
    for (int i = 0; i < image_width; i++) {
      color pixel_color;
      for (int k = 0; k < samples_per_pixel; k++) {
        auto u = double(i + random_double()) / (image_width-1); // 0 <= u,v < 1
        auto v = double(j + random_double()) / (image_height-1);
        ray r = cam.get_ray(u, v);
        pixel_color += ray_color(r, world, max_depth);
      }
      write_color(std::cout, pixel_color, samples_per_pixel);
    }
  }

  std::cerr << "\nDone!" << std::endl;
}
