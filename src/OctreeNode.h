#ifndef OCTREENODE_H
#define OCTREENODE_H

#include <list>
#include <memory>
#include <vector>
#include <queue>

#include "types.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class OctreeNode {
   public:

   // root node
   OctreeNode();

   // child node
   OctreeNode(OctreeNode *parent, glm::vec3 &minCoord, glm::vec3 &maxCoord);

   ~OctreeNode();

   void addObject(std::shared_ptr <surfel_t> obj);

   void buildTree();

   std::vector<std::shared_ptr<surfel_t>> intersection(glm::vec3 rayOrigin, glm::vec3 rayDirection);

   static bool boxIntersection(glm::vec3 boxMin, glm::vec3 boxMax, glm::vec3 rayOrigin, glm::vec3 rayDirection);

   glm::vec3 getMin() {return min;};
   glm::vec3 getMax() {return  max;};

   void printTree();
   void printTree(int level);
   void setDebugColor();


   private:

   OctreeNode* parent_;

   std::vector<OctreeNode> children_;

   glm::vec3 min;
   glm::vec3 max;

   std::vector<std::shared_ptr<surfel_t>> surfelsEnclosed_;

   std::list<std::shared_ptr<surfel_t>> surfelsNotInNode_;

   void createMinMaxForRoot();

   void buildTreeNode();

   bool contains(const std::shared_ptr<surfel_t> surfel);
};


#endif
