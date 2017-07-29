#include "Helper.h"

#include <math.h>
#include <algorithm>
#include <iostream>

glm::vec3 Helper::rotationAxis(glm::vec3 from, glm::vec3 to) {
   if(from == to) {
      return glm::vec3(0.0, 1.0, 0.0);
   } else if (from == -to) {
      if(from == glm::vec3(0.0, 0.0, 1.0)){
         return glm::cross(from, glm::vec3(0.0, 0.0, 1.0));
      }
      return glm::cross(from, glm::vec3(1.0, 0.0, 0.0));
   }

   return glm::cross(from, to);
}

float Helper::rotationAngle(glm::vec3 from, glm::vec3 to) {
   if(from == to) {
      return 0.0;
   } else if (from == -to) {
      return M_PI;
   }

   return atan2(glm::length(glm::cross(from, to)), glm::dot(from, to));
}

float Helper::RandomFloat(float a, float b) {
   float random = ((float) rand()) / (float) RAND_MAX;
   float diff = b - a;
   float r = random * diff;
   return a + r;
}

glm::vec3 Helper::randomUnitDiscPoint() {
   float theta = 2 * M_PI * Helper::RandomFloat(0.0, 1.0);
   float u = Helper::RandomFloat(0.0, 1.0) + Helper::RandomFloat(0.0, 1.0);
   float rad =  u;
   if(u > 1.0) {
      rad = 2.0 - u;
   }
   return glm::vec3(rad * cos(theta), 0.0, rad * sin(theta));
}

glm::vec3 Helper::CosineSampleHemisphere()
{
   float u = Helper::RandomFloat(0.0, 1.0);
   float r = sqrt(u);
   float theta = 2 * M_PI * Helper::RandomFloat(0.0, 1.0);

   float x = r * cos(theta);
   float z = r * sin(theta);

   return glm::vec3(x, sqrt(std::max(0.0f, 1 - u)), z);
}

bool Helper::intersectPlane(glm::vec3 &normal, glm::vec3 &planePoint, glm::vec3 &rayOrigin, glm::vec3 &rayDirection, float &t)
{
   float denom = glm::dot(normal, rayDirection);

   if (fabs(denom) > 0.0000001) {
      glm::vec3 planeRay = planePoint - rayOrigin;
      t = glm::dot(planeRay, normal) / denom;
      return (t >= 0);
   }

   return false;
}

bool Helper::intersectDisk(glm::vec3 &normal, glm::vec3 &center, float &radius, glm::vec3 &rayOrigin, glm::vec3 &rayDirection, glm::vec3 &intersectionPoint)
{
   float t = 0;
   if (Helper::intersectPlane(normal, center, rayOrigin, rayDirection, t)) {
      glm::vec3 intersectionPoint = rayOrigin + rayDirection * t;

      glm::vec3 dist = intersectionPoint - center;
      return (glm::length(dist) <= radius);
   }

   return false;
}

color_t Helper::glmToColorStruct(glm::vec3 color) {
   color_t colorS;
   colorS.r = color.r;
   colorS.g = color.g;
   colorS.b = color.b;
   return colorS;
}