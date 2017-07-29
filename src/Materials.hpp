#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <unordered_map>

typedef struct material_struct {
   glm::vec3 ambient;
   glm::vec3 diffuse;
   glm::vec3 specular;
   float shine;
} material_t;

class Materials {
public:
   ~Materials();
   static Materials& instance();
   std::shared_ptr<material_t> getMaterial(const std::string& mat);

private:
   Materials();

   // Map of available materials
   std::unordered_map<std::string, std::shared_ptr<material_t>> materials;
};

#endif
