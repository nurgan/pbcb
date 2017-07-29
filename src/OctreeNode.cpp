#include "OctreeNode.h"

#include <limits>

#include <iostream>

#include "Helper.h"


OctreeNode::OctreeNode() {
   parent_ = NULL;
}

OctreeNode::OctreeNode(OctreeNode* parent,  glm::vec3& minCoord, glm::vec3& maxCoord) {
   parent_ = parent;
   min = minCoord;
   max = maxCoord;
}

OctreeNode::~OctreeNode() {

}

void OctreeNode::addObject(std::shared_ptr<surfel_t> obj) {
   surfelsNotInNode_.push_front(obj);
}

void OctreeNode::buildTree() {
   createMinMaxForRoot();
   buildTreeNode();
}

void OctreeNode::createMinMaxForRoot() {
   min = glm::vec3(std::numeric_limits<float>::max());
   max = glm::vec3(-std::numeric_limits<float>::max());

   for (std::shared_ptr<surfel_t> surfel : surfelsNotInNode_) {

         glm::vec3 rad = glm::vec3(surfel->radius);
         glm::vec3 surfelMin = surfel->position - rad;
         glm::vec3 surfelMax = surfel->position + rad;

         min.x = surfelMin.x < min.x ? surfelMin.x : min.x;
         min.y = surfelMin.y < min.y ? surfelMin.y : min.y;
         min.z = surfelMin.z < min.z ? surfelMin.z : min.z;

         max.x = surfelMax.x > max.x ? surfelMax.x : max.x;
         max.y = surfelMax.y > max.y ? surfelMax.y : max.y;
         max.z = surfelMax.z > max.z ? surfelMax.z : max.z;
   }

}


std::vector<std::shared_ptr<surfel_t>> OctreeNode::intersection(glm::vec3 rayOrigin, glm::vec3 rayDirection) {

   std::vector<std::shared_ptr<surfel_t>> returnSufels;

   bool intersects = boxIntersection(min, max, rayOrigin, rayDirection);

   if(!intersects) {
      return returnSufels;
   }

   for(OctreeNode& kiddo : children_) {
      std::vector<std::shared_ptr<surfel_t>> recursionReturn = kiddo.intersection(rayOrigin, rayDirection);
      returnSufels.insert(returnSufels.end(), recursionReturn.begin(), recursionReturn.end());
   }

   returnSufels.insert(returnSufels.end(), surfelsEnclosed_.begin(), surfelsEnclosed_.end());

   return returnSufels;

}

bool OctreeNode::boxIntersection(glm::vec3 boxMin, glm::vec3 boxMax, glm::vec3 rayOrigin, glm::vec3 rayDirection) {
   if(rayDirection.x == 0) {
      rayDirection.x = std::numeric_limits<float>::min();
   }
   if(rayDirection.y == 0) {
      rayDirection.y = std::numeric_limits<float>::min();
   }
   if(rayDirection.z == 0) {
      rayDirection.z = std::numeric_limits<float>::min();
   }

   float t1 = (boxMin.x - rayOrigin.x) / rayDirection.x;
   float t2 = (boxMax.x - rayOrigin.x) / rayDirection.x;
   float t3 = (boxMin.y - rayOrigin.y) / rayDirection.y;
   float t4 = (boxMax.y - rayOrigin.y) / rayDirection.y;
   float t5 = (boxMin.z - rayOrigin.z) / rayDirection.z;
   float t6 = (boxMax.z - rayOrigin.z) / rayDirection.z;

   float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
   float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));


   // intersection is "behind" the camera.
   if (tmax < 0)
   {
      return false; //false
   }

   if (tmin > tmax)
   {
      return false; //false
   }

   return true;
}


bool OctreeNode::contains(const std::shared_ptr<surfel_t> surfel) {

   glm::vec3 rad = glm::vec3(surfel->radius, surfel->radius, surfel->radius);
   glm::vec3 surfelMin = surfel->position - rad;
   glm::vec3 surfelMax = surfel->position + rad;

   if (min.x <= surfelMin.x
       && min.z <= surfelMin.z
       && min.y <= surfelMin.y
       && max.x >= surfelMax.x
       && max.y >= surfelMax.y
       && max.z >= surfelMax.z) { ;

      return true;
   }

   return false;
}

void OctreeNode::printTree(int level) {

   for(int i = 0; i <= level; i++) {
      std:: cout << "  ";
   }
   std::cout << "Level " << level << " Node. Surfels: " << surfelsEnclosed_.size() << std::endl;
   for(int i = 0; i <= level; i++) {
      std:: cout << "  ";
   }
   std::cout << "min x: " << min.x << " y: " << min.y << " z: " << min.z;
   for(int i = 0; i <= level; i++) {
      std:: cout << "  ";
   }
   std::cout << "max x: " << max.x << " y: " << max.y << " z: " << max.z << std::endl;
   int i = 1;
   for (std::shared_ptr<surfel_t> surfel : surfelsEnclosed_) {
      for(int i = 0; i <= level; i++) {
         std:: cout << "  ";
      }
      std::cout << "surfel " << i << " x: " << surfel->position.x << " y: " << surfel->position.y << " z: " << surfel->position.z << std::endl;
      i++;
   }
   for (OctreeNode& child : children_) {
      child.printTree(level + 1);
   }

}

void OctreeNode::printTree() {
   std::cout << "Root Node. Surfels: " << surfelsEnclosed_.size() << std::endl;
   std::cout << "min x: " << min.x << " y: " << min.y << " z: " << min.z;
   std::cout << "max x: " << max.x << " y: " << max.y << " z: " << max.z << std::endl;
   int i = 1;
   for (std::shared_ptr<surfel_t> surfel : surfelsEnclosed_) {
      std::cout << "surfel " << i << " x: " << surfel->position.x << " y: " << surfel->position.y << " z: " << surfel->position.z << std::endl;
      i++;
   }
   for (OctreeNode& child : children_) {
         child.printTree(1);
   }
}


void OctreeNode::buildTreeNode() {

   glm::vec3 regionDim(max - min);
   glm::vec3 halfRegionDim(regionDim / 2.0f);
   glm::vec3 centerOfRegion(min + halfRegionDim);


   // If 16 or less surfels, put everything left in here
   if (surfelsNotInNode_.size() <= 16) {
      for (std::shared_ptr<surfel_t> surfel : surfelsNotInNode_) {
         surfelsEnclosed_.push_back(surfel);
      }
      surfelsNotInNode_.clear();

      return;
   }

   glm::vec3 childMin;
   glm::vec3 childMax;

   //1
   childMin = min;
   childMax = centerOfRegion;
   children_.emplace_back(this, childMin, childMax);
   //2
   childMin = glm::vec3(centerOfRegion.x, min.y, min.z);
   childMax = glm::vec3(max.x, centerOfRegion.y, centerOfRegion.z);
   children_.emplace_back(this, childMin, childMax);
   //3
   childMin = glm::vec3(centerOfRegion.x, min.y, centerOfRegion.z);
   childMax = glm::vec3(max.x, centerOfRegion.y, max.z);
   children_.emplace_back(this, childMin,childMax);
   //4
   childMin = glm::vec3(min.x, min.y, centerOfRegion.z);
   childMax = glm::vec3(centerOfRegion.x, centerOfRegion.y, max.z);
   children_.emplace_back(this, childMin, childMax);
   //5
   childMin = glm::vec3(min.x, centerOfRegion.y, min.z);
   childMax = glm::vec3(centerOfRegion.x, max.y, centerOfRegion.z);
   children_.emplace_back(this, childMin, childMax);
   //6
   childMin = glm::vec3(centerOfRegion.x, centerOfRegion.y, min.z);
   childMax = glm::vec3(max.x, max.y, centerOfRegion.z);
   children_.emplace_back(this, childMin, childMax);
   //7
   childMin = centerOfRegion;
   childMax = max;
   children_.emplace_back(this, childMin, childMax);
   //8
   childMin = glm::vec3(min.x, centerOfRegion.y, centerOfRegion.z);
   childMax = glm::vec3(centerOfRegion.x, max.y, max.z);
   children_.emplace_back(this, childMin, childMax);

   std::queue<std::shared_ptr<surfel_t>> objsRemoved;

   // Find all objects that fit perfectly in the child node
   for (std::shared_ptr<surfel_t> surfel : surfelsNotInNode_) {
      for (OctreeNode& child : children_) {
         if (child.contains(surfel)) {

            child.surfelsNotInNode_.push_front(surfel);
            objsRemoved.push(surfel);
            break;
         }
      }
   }

   while (!objsRemoved.empty()) {
      std::shared_ptr<surfel_t> objToRemove = objsRemoved.front();
      objsRemoved.pop();
      surfelsNotInNode_.remove(objToRemove);
   }

   for (std::shared_ptr<surfel_t> surfel : surfelsNotInNode_) {
      surfelsEnclosed_.push_back(surfel);
   }

   surfelsNotInNode_.clear();

   for (OctreeNode& child : children_) {
      if (child.surfelsNotInNode_.size() > 0) {
         child.buildTreeNode();
      }
   }
}
