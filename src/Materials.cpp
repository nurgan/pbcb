#include "Materials.hpp"
#include <iostream>

Materials& Materials::instance() {
   static Materials *instance = new Materials();
   return *instance;
}

Materials::Materials() {
   // Populate the `materials` map
   std::shared_ptr<material_t> mat;
   std::pair<std::string, std::shared_ptr<material_t>> material;

   // Emerald
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.0215f, 0.1745f, 0.0215f);             
   mat->diffuse = glm::vec3(0.07568f, 0.61424f, 0.07568f);          
   mat->specular = glm::vec3(0.633f, 0.727811f, 0.633f);            
   mat->shine = 76.8f;
   material = std::make_pair("emerald", mat);
   materials.insert(material);

   // Shiny Blue Plastic
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.02f, 0.04f, 0.2f);             
   mat->diffuse = glm::vec3(0.0f, 0.16f, 0.9f);          
   mat->specular = glm::vec3(0.14f, 0.2f, 0.8f);            
   mat->shine = 120.0f;
   material = std::make_pair("blue plastic", mat);
   materials.insert(material);

   // Flat Grey
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.13, 0.13, 0.14);
   mat->diffuse = glm::vec3(0.3, 0.3, 0.4);
   mat->specular = glm::vec3(0.3, 0.3, 0.4);
   mat->shine = 4.0;
   material = std::make_pair("matte grey", mat);
   materials.insert(material);

   // Brass
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.3294, 0.2235, 0.02745);
   mat->diffuse = glm::vec3(0.7804, 0.5686, 0.11373);
   mat->specular = glm::vec3(0.9922, 0.941176, 0.80784);
   mat->shine = 27.9;
   material = std::make_pair("brass", mat);
   materials.insert(material);

   // Copper
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.1913, 0.0735, 0.0225);
   mat->diffuse = glm::vec3(0.7038, 0.27048, 0.0828);
   mat->specular = glm::vec3(0.257, 0.1376, 0.08601);
   mat->shine = 12.8;
   material = std::make_pair("copper", mat);
   materials.insert(material);

   // Ruby
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.1745, 0.01175, 0.01175);
   mat->diffuse = glm::vec3(0.61424, 0.04136, 0.04136);
   mat->specular = glm::vec3(0.727811, 0.626959, 0.626959);
   mat->shine = 76.8;
   material = std::make_pair("ruby", mat);
   materials.insert(material);

   // Obsidian
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.05375, 0.05, 0.06625); 
   mat->diffuse = glm::vec3(0.18275, 0.17, 0.22525);
   mat->specular = glm::vec3(0.332741, 0.328634, 0.346435);
   mat->shine = 38.4;
   material = std::make_pair("obsidian", mat);
   materials.insert(material);

   // Cyan Plastic
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.0, 0.3, 0.3);
   mat->diffuse = glm::vec3(0.0, 0.50980392, 0.50980392);
   mat->specular = glm::vec3(0.50196078, 0.50196078, 0.50196078);
   mat->shine = 32.0;
   material = std::make_pair("cyan plastic", mat);
   materials.insert(material);

   // Yellow Plastic
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.3, 0.3, 0.0);
   mat->diffuse = glm::vec3(0.5, 0.5, 0.0);
   mat->specular = glm::vec3(0.6, 0.6, 0.5);
   mat->shine = 32.0;
   material = std::make_pair("yellow plastic", mat);
   materials.insert(material);


   // Red Rubber
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.05, 0.0, 0.0);
   mat->diffuse = glm::vec3(0.5, 0.4, 0.4);
   mat->specular = glm::vec3(0.7, 0.04, 0.04);
   mat->shine = 10.0;
   material = std::make_pair("red rubber", mat);
   materials.insert(material);

   // White Plastic
   mat = std::make_shared<material_t>();
   mat->ambient = glm::vec3(0.2, 0.3, 0.3);
   mat->diffuse = glm::vec3(0.5, 0.5, 0.5);
   mat->specular = glm::vec3(0.7, 0.7, 0.7);
   mat->shine = 75.0;
   material = std::make_pair("white plastic", mat);
   materials.insert(material);
}

std::shared_ptr<material_t> Materials::getMaterial(const std::string& mat) {
   return materials.at(mat);
}
