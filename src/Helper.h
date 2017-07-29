#ifndef HELPER_H
#define HELPER_H

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "types.h"

class Helper {
   public:

   static glm::vec3 rotationAxis(glm::vec3 from, glm::vec3 to);

   static float rotationAngle(glm::vec3 from, glm::vec3 to);

   static float RandomFloat(float a, float b);

   static glm::vec3 randomUnitDiscPoint();

   static glm::vec3 CosineSampleHemisphere();

   static bool intersectPlane(glm::vec3 &normal, glm::vec3 &planePoint, glm::vec3 &rayOrigin, glm::vec3 &rayDirection, float &t);

   static bool intersectDisk(glm::vec3 &normal, glm::vec3 &center, float &radius, glm::vec3 &rayOrigin, glm::vec3 &rayDirection, glm::vec3 &intersectionPoint);

   static color_t glmToColorStruct(glm::vec3 color);

};

#endif
