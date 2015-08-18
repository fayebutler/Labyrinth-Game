#ifndef COLLISIONSHAPE_H__
#define COLLISIONSHAPE_H__

//----------------------------------------------------------------------------------------------------------------------
/// @file CollisionShape.h
/// @brief simple class to store instances of bullet collision
/// shapes this is a singleton so we only have one instance of
/// each shape.
//----------------------------------------------------------------------------------------------------------------------

#include <ngl/Singleton.h>
#include <btBulletDynamicsCommon.h>
#include <map>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
/// @class CollisionShape "include/CollisionShape.h"
/// @brief Class to create the collisoin shapes for the objects in physics world
/// @author Faye Butler
/// Modified from - Jon Macey
/// @date 10/04/2014
//----------------------------------------------------------------------------------------------------------------------

class CollisionShape
{
public :
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief create an instance of a collision shape
  //----------------------------------------------------------------------------------------------------------------------
  static CollisionShape *instance();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief create collision shape for a sphere
  /// @param[in] name of shape as a string
  /// @param[in] file path to the obj mesh as a string
  //----------------------------------------------------------------------------------------------------------------------
  void addSphere(const std::string & _name, const std::string &_objFilePath);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief create collision shape for maze
  /// @param[in] name of shape as a string
  /// @param[in] file path to the obj mesh as a string
  //----------------------------------------------------------------------------------------------------------------------
  void addMaze(const std::string & _name, const std::string &_objFilePath);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief create collision shape for box
  /// @param[in] name of shape as a string
  /// @param[in] file path to the obj mesh as a string
  //----------------------------------------------------------------------------------------------------------------------
  void addBox(const std::string & _name, const std::string &_objFilePath);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief returns collision shape
  /// @param[in] name of shape as a string
  //----------------------------------------------------------------------------------------------------------------------
  btCollisionShape* getShape(const std::string &_name);

protected :
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief ctor but unused
  //----------------------------------------------------------------------------------------------------------------------
  CollisionShape(){}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief map to hold collision shapes and names
  //----------------------------------------------------------------------------------------------------------------------
  std::map <std::string,btCollisionShape*> m_shapes;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief collision shape instance
  //----------------------------------------------------------------------------------------------------------------------
  static CollisionShape *s_instance;
};



#endif
