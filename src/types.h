#ifndef __TYPES_H__
#define __TYPES_H__

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

/* Color struct */
typedef struct color_struct {
   double r;
   double g;
   double b;
   double f; // "filter" or "alpha"
} color_t;

typedef struct surfel_struct {
   glm::vec3 color;
   glm::vec3 position;
   glm::vec3 normal;
   float radius;
} surfel_t;

enum geometry_type {sphere, plane};

typedef struct geometry_struct {
   geometry_type type;
   glm::vec3 position;
   float radius;
   glm::vec3 normal;
   float extends;
   std::string material;
} geometry_t;


#endif
