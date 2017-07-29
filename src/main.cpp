#include <iostream>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <limits>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Image.h"
#include "types.h"
#include "Materials.hpp"

#include "OctreeNode.h"
#include "Helper.h"
#include <glm/gtx/rotate_vector.hpp>

#define EPSILON 0.00001f

std::vector<glm::vec3> lightsPos;
std::vector<glm::vec3> lightsCol;

std::vector<geometry_struct> geometry;

int surW = 40, surH = 30;
float surfelRadius = 0.5f;
int pixW, pixH;

int hemisphereRays = 20;
//int hemisphereRays = 64;
//int hemisphereRays = 128;

float surfelLeft, surfelRight, surfelTop, surfelBottom;
float pixelLeft, pixelRight, pixelTop, pixelBottom;

glm::vec3 eye = glm::vec3(0, 0, 5);

// find uvw-coordinate system
glm::vec3 Eu = glm::vec3(1, 0, 0);
glm::vec3 Ev = glm::vec3(0, 1, 0);
glm::vec3 Ew = glm::vec3(0, 0, 1);

std::vector<glm::vec3> hemisphere;

OctreeNode octree;

// point cloud data
std::vector<std::shared_ptr<surfel_t>> surfels;


glm::vec3 calcPhong(glm::vec3 ray_dir, glm::vec3 point, glm::vec3 normal, std::string material) {

   Materials& materials = Materials::instance();
   std::shared_ptr<material_t> mat = materials.getMaterial(material);

   glm::vec3 color;

   for(int i = 0; i < lightsPos.size(); i++) {
      glm::vec3 lightPos = lightsPos[i];
      glm::vec3 lightColor = lightsCol[i];
      glm::vec3 lightVec = glm::normalize(lightPos - point);

      glm::vec3 diffuse = mat->diffuse *
                          glm::max(glm::dot(normalize(normal), lightVec), 0.0f) * lightColor;

      glm::vec3 view = glm::normalize(-ray_dir);
      glm::vec3 surfNorm = normalize(normal);
      glm::vec3 halfVec = (lightVec + view) / glm::length(lightVec + view);
      glm::vec3 specular = mat->specular
                           * glm::pow(glm::max(glm::dot(surfNorm, halfVec), 0.0f), mat->shine)
                           * lightColor;

      glm::vec3 ambient = mat->ambient * lightColor;

      glm::vec3 result = diffuse + specular + ambient;

      color += result;
   }

   return color;
}


void init() {

   if (surW > surH) {
      surfelLeft = -1 * ((float)surW / surH);
      surfelRight = (float)surW / surH;
      surfelTop = 1;
      surfelBottom = -1;
   } else {
      surfelLeft = -1;
      surfelRight = 1;
      surfelTop = (float)surH / surW;
      surfelBottom = -1 * ((float)surH / surW);
   }

   if (pixW > pixH) {
      pixelLeft = -1 * ((float)pixW / pixH);
      pixelRight = (float)pixW / pixH;
      pixelTop = 1;
      pixelBottom = -1;
   } else {
      pixelLeft = -1;
      pixelRight = 1;
      pixelTop = (float)pixH / pixW;
      pixelBottom = -1 * ((float)pixH / pixW);
   }

   for (int i = 0; i < hemisphereRays; i ++) {
      glm::vec3 randomPoint = Helper::CosineSampleHemisphere();
      hemisphere.push_back(randomPoint);
   }

   // 2 sphres

   geometry_struct g1;
   g1.type = geometry_type::sphere;
   g1.position = glm::vec3(-3.5, -4, -5);
   g1.radius = 3.0f;
   g1.material = "yellow plastic";

   geometry_struct g2;
   g2.type = geometry_type::sphere;
   g2.position = glm::vec3(3.5, -4, -5);
   g2.radius = 3.0f;
   g2.material = "cyan plastic";

   geometry.push_back(g1);
   geometry.push_back(g2);

   // 5 planes that make up the box

   geometry_struct g3;
   g3.type = geometry_type::plane;
   g3.position = glm::vec3(-7, 0, -3);
   //g3.extends = 6.0f;
   g3.extends = 7.1f;
   g3.normal = glm::vec3(1.0f, 0.0f, 0.0f);
   g3.material = "ruby";

   geometry.push_back(g3);

   geometry_struct g4;
   g4.type = geometry_type::plane;
   g4.position = glm::vec3(7, 0, -3);
   //g4.extends = 6.0f;
   g4.extends = 7.1f;
   g4.normal = glm::vec3(-1.0f, 0.0f, 0.0f);
   g4.material = "emerald";

   geometry.push_back(g4);

   geometry_struct g5;
   g5.type = geometry_type::plane;
   g5.position = glm::vec3(0, -7, -3);
   //g5.extends = 6.0f;
   g5.extends = 7.1f;
   g5.normal = glm::vec3(0.0f, 1.0f, 0.0f);
   g5.material = "white plastic";

   geometry.push_back(g5);

   geometry_struct g6;
   g6.type = geometry_type::plane;
   g6.position = glm::vec3(0, 7, -3);
   //g3.extends = 6.0f;
   g6.extends = 7.1f;
   g6.normal = glm::vec3(0.0f, -1.0f, 0.0f);
   g6.material = "white plastic";

   geometry.push_back(g6);

   geometry_struct g7;
   g7.type = geometry_type::plane;
   g7.position = glm::vec3(0, 0, -9);
   //g3.extends = 6.0f;
   g7.extends = 7.1f;
   g7.normal = glm::vec3(0.0f, 0.0f, 1.0f);
   g7.material = "white plastic";

   geometry.push_back(g7);

   lightsPos.push_back(glm::vec3(0.0f, 6.0f, -3.0f));
   lightsCol.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

}


void createSphereSurfel(geometry_struct geom, glm::vec3 s, glm::vec3 d) {

   glm::vec3 normal;
   glm::vec3 color;

   glm::vec3 sphere =geom.position;
   float radius = geom.radius;
   std::string material = geom.material;

   float discrim = pow(dot(d, eye - sphere), 2) - (dot(d, d) * (
         dot(eye - sphere, eye - sphere) - pow(radius, 2)));

   // The ray intersects with the sphere color it
   if (discrim >= 0.0f) {
      float t1, t2;
      glm::vec3 point;

      t1 = (-dot(d, eye - sphere) + sqrt(discrim)) / dot(d, d);
      t2 = (-dot(d, eye - sphere) - sqrt(discrim)) / dot(d, d);

      // if only one solution aka grazing the sphere, calculate that surfel
      if ((abs(t1) - abs(t2)) < EPSILON) {
         point = eye + t1 * d;
         normal = 2.0f * (point - sphere);
         color = calcPhong(d, point, normal, material);
         std::shared_ptr<surfel_t> surfel = std::make_shared<surfel_t>();
         surfel->color = color;
         surfel->position = point;
         surfel->radius = surfelRadius;
         surfel->normal = glm::normalize(normal);
         surfels.push_back(surfel);
         // if two solutions aka entry and exit point, calculate these surfels
      } else {
         point = eye + t1 * d;
         normal = 2.0f * (point - sphere);
         color = calcPhong(d, point, normal, material);
         std::shared_ptr<surfel_t> surfel1 = std::make_shared<surfel_t>();
         surfel1->color = color;
         surfel1->position = point;
         surfel1->radius = surfelRadius;
         surfel1->normal = glm::normalize(normal);
         surfels.push_back(surfel1);

         point = eye + t2 * d;
         normal = 2.0f * (point - sphere);
         color = calcPhong(d, point, normal, material);
         std::shared_ptr<surfel_t> surfel2 = std::make_shared<surfel_t>();
         surfel2->color = color;
         surfel2->position = point;
         surfel2->radius = surfelRadius;
         surfel2->normal = glm::normalize(normal);
         surfels.push_back(surfel2);
      }
   }
}


void createPlaneSurfel(geometry_struct geom, glm::vec3 s, glm::vec3 d) {

   glm::vec3 color;
   float t;

   if (Helper::intersectPlane(geom.normal, geom.position, s, d, t)) {
      glm::vec3 surfelPos = s + d * t;
      glm::vec3 dist = surfelPos - geom.position;

      //WARNING: only works for planes that have a coordinate-axis as normal. Sufficient for this project.
      if(abs(dist.x) < geom.extends && abs(dist.y) < geom.extends && abs(dist.z) < geom.extends) {
         color = calcPhong(d, surfelPos, geom.normal, geom.material);
         std::shared_ptr<surfel_t> surfel = std::make_shared<surfel_t>();
         surfel->color = color;
         surfel->position = surfelPos;
         surfel->radius = surfelRadius;
         surfel->normal = glm::normalize(geom.normal);
         surfels.push_back(surfel);
      }
   }
}


void createSurfels() {

   // uvw-coordinate scalars
   float Su, Sv, Sw;
   // Screen space point and travel vector
   glm::vec3 s;
   glm::vec3 d;
   color_t colorS;

   for (int i = 0; i < surW; i++) {
      for (int j = 0; j < surH; j++) {
         // Calculate uvw scalars
         Su = surfelLeft + (surfelRight - surfelLeft) * ((i + 0.5) / (float) surW);
         Sv = surfelBottom + (surfelTop - surfelBottom) * ((j+ 0.5) / (float) surH);
         Sw = -1;

         s = eye + (Su * Eu) + (Sv * Ev) + (Sw * Ew);
         d = s - eye;

         for(int k = 0; k < geometry.size(); k++) {
            if(geometry[k].type == geometry_type::sphere) {
               createSphereSurfel(geometry[k], s, d);
            } else {
               createPlaneSurfel(geometry[k], s, d);
            }
         }
      }
   }
}


glm::vec3 traceSurfels(glm::vec3 rayOrigin, glm::vec3 rayDirection, std::vector<std::shared_ptr<surfel_t>> surfels) {

   glm::vec3 returnColor = glm::vec3(0.0f, 0.0f, 0.0f);
   float lastDist = std::numeric_limits<float>::lowest();

   for(int k = 0; k < surfels.size(); k++) {

         glm::vec3 intersectionPoint;
         // The ray intersects with the surfel
         if (Helper::intersectDisk(surfels[k]->normal, surfels[k]->position, surfels[k]->radius, rayOrigin,
                                   rayDirection, intersectionPoint)) {

            float dist = glm::length(intersectionPoint - rayOrigin);
            if (lastDist > dist) {
               returnColor = surfels[k]->color;
               lastDist = dist;
            }
         }
   }

   return returnColor;
}


glm::vec3 calcColorBleeding(glm::vec3 currentPoint, glm::vec3 currentNormal) {

   currentNormal = glm::normalize(currentNormal);
   glm::vec3 yDir = glm::vec3(0.0, 1.0, 0.0);
   glm::vec3 rAxis = Helper::rotationAxis(yDir, currentNormal);
   float rAngle = Helper::rotationAngle(yDir, currentNormal);
   glm::vec3 surfelColor = glm::vec3(0.0f);

   for(int l = 0; l < hemisphere.size(); l++) {
      glm::vec3 ray = glm::normalize(glm::rotate(hemisphere[l], rAngle, rAxis));
      std::vector<std::shared_ptr<surfel_t>> octreeSurfels = octree.intersection(currentPoint, ray);
      glm::vec3 tracedSurfelColor = traceSurfels(currentPoint, ray, octreeSurfels);

      surfelColor += tracedSurfelColor;
   }

   surfelColor = surfelColor / ((float)hemisphere.size());

   return surfelColor;
}


void raytraceSphere(glm::vec3 d, glm::vec3 position, float radius, std::string material, float &lastZ, color_t &colorS) {

   glm::vec3 normal;
   glm::vec3 color;

   float discrim = pow(dot(d, eye - position), 2) - (dot(d, d) * (
         dot(eye - position, eye - position) - pow(radius, 2)));

   // The ray intersects with the sphere color it
   if ( discrim >= 0.0f) {
      float t1, t2;
      glm::vec3 point;
      glm::vec3 tmpPoint;

      t1 = (-dot(d, eye - position) + sqrt(discrim)) / dot(d, d);
      t2 = (-dot(d, eye - position) - sqrt(discrim)) / dot(d, d);

      if (t1 <= t2) {
         tmpPoint = eye + t1 * d;
         if(tmpPoint.z > lastZ) {
            point = tmpPoint;
            normal = 2.0f * (point - position);
            color = calcPhong(d, point, normal, material);
            colorS = Helper::glmToColorStruct(color);
            lastZ = point.z;
            //offset point so it does't intersect with "its own" surfel
            point += 0.1f * glm::normalize(normal);

            glm::vec3 colorBleedingColor = calcColorBleeding(point, normal);
            colorS.r += colorBleedingColor.r;
            colorS.g += colorBleedingColor.g;
            colorS.b += colorBleedingColor.b;
         }
      } else {
         tmpPoint = eye + t2 * d;
         if(tmpPoint.z > lastZ) {
            point = tmpPoint;
            normal = 2.0f * (point - position);
            color = calcPhong(d, point, normal, material);
            colorS = Helper::glmToColorStruct(color);
            lastZ = point.z;
            //offset point so it does't intersect with "its own" surfel
            point += 0.1f * glm::normalize(normal);

            glm::vec3 colorBleedingColor = calcColorBleeding(point, normal);
            colorS.r += colorBleedingColor.r;
            colorS.g += colorBleedingColor.g;
            colorS.b += colorBleedingColor.b;
         }
      }
   }
}


void raytracePlane(glm::vec3 s, glm::vec3 d, geometry_t geom, std::string material, float &lastZ, color_t &colorS) {

   glm::vec3 color;
   float t;

   if (Helper::intersectPlane(geom.normal, geom.position, s, d, t)) {
      glm::vec3 point = s + d * t;
      if(point.z > lastZ) {
         glm::vec3 dist = point - geom.position;
         //WARNING: only works for planes that have a coordinate-axis as normal. Sufficient for me tho.
         if (abs(dist.x) < geom.extends && abs(dist.y) < geom.extends && abs(dist.z) < geom.extends) {
            color = calcPhong(d, point, geom.normal, geom.material);
            colorS = Helper::glmToColorStruct(color);
            lastZ = point.z;
            //offset point so it does't intersect with "its own" surfel
            point += 0.1f * glm::normalize(geom.normal);
            glm::vec3 colorBleedingColor = calcColorBleeding(point, geom.normal);
            colorS.r += colorBleedingColor.r;
            colorS.g += colorBleedingColor.g;
            colorS.b += colorBleedingColor.b;
         }
      }
   }
}


void renderSurfels(Image *img){

   glm::vec3 normal;
   glm::vec3 color;
   float Su, Sv, Sw;
   color_t colorS;
   // Screen space point and travel vector
   glm::vec3 s;
   glm::vec3 d;

   for (int i = 0; i < pixW; i++) {

      std::cout << (((float)i) / ((float)pixW)) * 100.0f << " % done " << std::endl;

      for (int j = 0; j < pixH; j++) {
         colorS.r = colorS.g = colorS.b = 0.0f;

         float lastZ = -999999.0f;

         glm::vec3 rayDir = glm::vec3(
               pixelLeft + (pixelRight - pixelLeft) * ((i + 0.5) / (float) pixW),
               pixelBottom + (pixelTop - pixelBottom) * ((j + 0.5) / (float) pixH),
               -1.0);
         Su = pixelLeft + (pixelRight - pixelLeft) * ((i + 0.5) / (float) pixW);
         Sv = pixelBottom + (pixelTop - pixelBottom) * ((j + 0.5) / (float) pixH);
         Sw = -1;
         s = eye + (Su * Eu) + (Sv * Ev) + (Sw * Ew);
         d = s - eye;
         rayDir = glm::normalize(d);

         std::vector<std::shared_ptr<surfel_t>> octreeSurfels = octree.intersection(eye, rayDir);

         for(int k = 0; k < octreeSurfels.size(); k++) {
            glm::vec3 sphere = octreeSurfels[k]->position;
            float radius = octreeSurfels[k]->radius;
            std::string material = "white plastic";

            float discrim = pow(dot(d, eye - sphere), 2) - (dot(d, d) * (
                  dot(eye - sphere, eye - sphere) - pow(radius, 2)));

            if ( discrim >= 0.0f) {
               float t1, t2;
               glm::vec3 point;
               glm::vec3 tmpPoint;

               t1 = (-dot(d, eye - sphere) + sqrt(discrim)) / dot(d, d);
               t2 = (-dot(d, eye - sphere) - sqrt(discrim)) / dot(d, d);

               if (t1 <= t2) {
                  tmpPoint = eye + t1 * d;
                  if(tmpPoint.z > lastZ) {
                     point = tmpPoint;
                     normal = 2.0f * (point - sphere);
                     colorS.r = octreeSurfels[k]->color.r;
                     colorS.g = octreeSurfels[k]->color.g;
                     colorS.b = octreeSurfels[k]->color.b;
                     lastZ = point.z;
                  }
               } else {
                  tmpPoint = eye + t2 * d;
                  if(tmpPoint.z > lastZ) {
                     point = tmpPoint;
                     normal = 2.0f * (point - sphere);
                     colorS.r = octreeSurfels[k]->color.r;
                     colorS.g = octreeSurfels[k]->color.g;
                     colorS.b = octreeSurfels[k]->color.b;
                     lastZ = point.z;
                  }
               }
            }
         }
         img->pixel(i, j, colorS);
      }
   }
}


void renderPicture(Image *img) {

   float Su, Sv, Sw;
   color_t colorS;
   // Screen space point and travel vector
   glm::vec3 s;
   glm::vec3 d;

   for (int i = 0; i < pixW; i++) {

      std::cout << (((float)i) / ((float)pixW)) * 100.0f << " % done " << std::endl;

      for (int j = 0; j < pixH; j++) {
         colorS.r = colorS.g = colorS.b = 0.0f;
         float lastZ = -999999.0f;

         glm::vec3 rayDir = glm::vec3(
               pixelLeft + (pixelRight - pixelLeft) * ((i + 0.5) / (float) pixW),
               pixelBottom + (pixelTop - pixelBottom) * ((j + 0.5) / (float) pixH),
               -1.0);
         Su = pixelLeft + (pixelRight - pixelLeft) * ((i + 0.5) / (float) pixW);
         Sv = pixelBottom + (pixelTop - pixelBottom) * ((j + 0.5) / (float) pixH);
         Sw = -1;
         s = eye + (Su * Eu) + (Sv * Ev) + (Sw * Ew);
         d = s - eye;
         rayDir = glm::normalize(d);

         for (int k = 0; k < geometry.size(); k++) {
            glm::vec3 sphere = geometry[k].position;
            float radius = geometry[k].radius;
            std::string material = geometry[k].material;

            if (geometry[k].type == geometry_type::sphere) {
               raytraceSphere(d, sphere, radius, material, lastZ, colorS);
            } else {
               raytracePlane(s, d, geometry[k], material, lastZ, colorS);
            }

            img->pixel(i, j, colorS);
         }
      }
   }
}


void displayHelp() {
   std::cout << "Please enter arguments in the following format:" << std::endl;
   std::cout << "./pbcb imageWidht imageHeight [optional surfel-mode] [optional render-mode]" << std::endl;
   std::cout << "surfel-mode: low, medium or heigh" << std::endl;
   std::cout << "render-mode: surfels to render the surfels instead of the actual image" << std::endl;
}


int main(int argc, char **argv) {

   if(argc < 3) {
      displayHelp();
      return 0;
   }

   pixH = std::atoi(argv[1]);
   pixW = std::atoi(argv[2]);

   bool displaySurfels = false;

   std::string surfelMode;
   if(argc >= 4) {
      surfelMode = argv[3];
      if(surfelMode == "low" || surfelMode == "l" || surfelMode == "-low" || surfelMode == "-l" || surfelMode == "--low" || surfelMode == "--l") {
         surW = 80;
         surH = 60;
         surfelRadius = 0.5f;
      } else if(surfelMode == "medium" || surfelMode == "m" || surfelMode == "-medium" || surfelMode == "-m" || surfelMode == "--medium" || surfelMode == "--m") {
         surW = 160;
         surH = 120;
         surfelRadius = 0.3f;
      } else if(surfelMode == "high" || surfelMode == "h" || surfelMode == "-high" || surfelMode == "-h" || surfelMode == "--high" || surfelMode == "--h") {
         surW = 320;
         surH = 240;
         surfelRadius = 0.1f;
      } else if (surfelMode == "surfels" || surfelMode == "-surfels" || surfelMode == "--surfels" || surfelMode == "s" || surfelMode == "-s" || surfelMode == "--s"){
         displaySurfels = true;
      } else {
         displayHelp();
         return 0;
      }
   }

   if(argc >= 5) {
      surfelMode = argv[4];
      if(surfelMode == "surfels" || surfelMode == "-surfels" || surfelMode == "--surfels" || surfelMode == "s" || surfelMode == "-s" || surfelMode == "--s") {
         displaySurfels = true;
      } else {
         displayHelp();
         return 0;
      }
   }

   init();

   createSurfels();

   std::cout << "# surfels: " << surfels.size() << std::endl;

   std::cout << "Starting to build octree" << std::endl;
   for(std::shared_ptr<surfel_t> surfel : surfels) {
      octree.addObject(surfel);
   }
   octree.buildTree();
   std::cout << "Finished to build octree" << std::endl;

   Image* img = new Image(pixW, pixH);

   // this renders the surfels instead of the actual image
   if(displaySurfels) {
      renderSurfels(img);
   } else {
      renderPicture(img);
   }

   img->WriteTga((char *)"output.tga", false);

}