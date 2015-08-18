#ifndef PHYSICSWORLD_H__
#define PHYSICSWORLD_H__

//----------------------------------------------------------------------------------------------------------------------
/// @file PhysicsWorld.h
/// @brief set physics for objects and physics world
//----------------------------------------------------------------------------------------------------------------------

#include <vector>
#include <btBulletDynamicsCommon.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Obj.h>

//----------------------------------------------------------------------------------------------------------------------
/// @class PhysicsWorld "include/PhysicsWorld.h"
/// @brief Class to set physics for the world an objects to be used in NGLDRaw
/// @author Faye Butler
/// Modified from - Jon Macey
/// @date 10/04/2014
//----------------------------------------------------------------------------------------------------------------------


class PhysicsWorld
{
public :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor, this should really be a singleton as we have quite a few static members and only one world
    //----------------------------------------------------------------------------------------------------------------------
    PhysicsWorld();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor
    //----------------------------------------------------------------------------------------------------------------------
    ~PhysicsWorld();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief reset the simulation (remove all geo etc)
    //----------------------------------------------------------------------------------------------------------------------
    void reset();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief remove rigid bodies
    /// @param[in] number of the rigid body in vector of bodies (m_bodies)
    //----------------------------------------------------------------------------------------------------------------------
    void removeBody(unsigned int _index);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set gravity
    /// @param[in] float for the x, y and z strength
    //----------------------------------------------------------------------------------------------------------------------
    inline void setGravity(float _x, float _y, float _z)
    {
      m_dynamicsWorld->setGravity(btVector3(_x,_y,_z));
    }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set gravity
    /// @param[in] vec3 containing x, y and z strength
    //----------------------------------------------------------------------------------------------------------------------
    inline void setGravity(ngl::Vec3 _g)
    {
      m_dynamicsWorld->setGravity(btVector3(_g.m_x,_g.m_y,_g.m_z));
    }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns string of the rigid body's name
    /// @param[in] number of the rigid body in vector of bodies (m_bodies)
    //----------------------------------------------------------------------------------------------------------------------
    inline std::string getBodyNameAtIndex(unsigned int i) const{return m_bodies[i].name;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief physics for the ground plane
    /// @param[in] vec3 to show x, y and z position of the plane
    /// @param[in] vec3 for the size of the plane (width, height depth)
    //----------------------------------------------------------------------------------------------------------------------
    void addGroundPlane(const ngl::Vec3 &_pos,const ngl::Vec3 &_size);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief physics for the maze
    /// @param[in] shape name as a string
    /// @param[in] position as vec3 (x,y,z)
    /// @param[in] friction (read from config file)
    //----------------------------------------------------------------------------------------------------------------------
    void addMaze(std::string _shapeName,const ngl::Vec3 &_pos, float _friction);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief physics for the ball
    /// @param[in] shape name as a string
    /// @param[in] position as vec3 (x,y,z)
    /// @param[in] friction (read from config file)
    //----------------------------------------------------------------------------------------------------------------------
    void addSphere(std::string _shapeName,const ngl::Vec3 &_pos, float _friction);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief physics for the cube (finish line)
    /// @param[in] shape name as a string
    /// @param[in] position as a vec3 (x,y,z)
    //----------------------------------------------------------------------------------------------------------------------
    void addCube(std::string _shapeName, const ngl::Vec3 &_pos);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief to step through the simulation
    /// @param[in] amount of time to step simulation by as a float (default 1/60th of asecond)
    /// @param[in] amount of time steps
    //----------------------------------------------------------------------------------------------------------------------
    void step(float _time, float _step);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief user pointer
    /// @param[in] number of collision object in array
    //----------------------------------------------------------------------------------------------------------------------
    void * getUserData(unsigned int _index);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns number of collision objects
    //----------------------------------------------------------------------------------------------------------------------
    inline unsigned int getNumCollisionObjects()const
    {
      return m_dynamicsWorld->getNumCollisionObjects();
    }
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief returns whether there is contact between two bodies (true/false)
    /// @param[in] number of the first specific collision object for contact test within array
    /// @param[in] number of the second specific collision object for contact test within array
    //----------------------------------------------------------------------------------------------------------------------
    bool contactTest(unsigned int i, unsigned int j);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief get position of a specific body
    /// @param[in] number of the specific collision object within array
    //----------------------------------------------------------------------------------------------------------------------
     ngl::Vec3 getPosition(unsigned int _index);
    //----------------------------------------------------------------------------------------------------------------------
     /// @brief get transform for specific body
     /// @param[in] number of the specific collision object within array
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 getTransformMatrix(unsigned int _index);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief get collision shape for specific body
    /// @param[in] number of specific collision object in array
    //----------------------------------------------------------------------------------------------------------------------
    int getCollisionShape(unsigned int _index) const;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set rotation for a specific body
    /// @param[in] number of specific collision object in array
    /// @param[in] quaternion for the rotation
    //----------------------------------------------------------------------------------------------------------------------
    void setRot(unsigned int _index, btQuaternion _rot);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief set rotation about the origin for a specific body
    /// @param[in] number of specific collision object in array
    /// @param[in] quaternion for the rotation
    //----------------------------------------------------------------------------------------------------------------------
    void setRotAboutOrigin(unsigned int _index, btQuaternion _rot);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief get rotation of a specific body
    /// @param[in] number of the specific collision object
    //----------------------------------------------------------------------------------------------------------------------
    btQuaternion getRotation(unsigned int _index);
    //----------------------------------------------------------------------------------------------------------------------

protected :
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief struct to record name and rigid body of objects
    //----------------------------------------------------------------------------------------------------------------------
    typedef struct
    {
      std::string name;
      btRigidBody* body;
    }Body;

    //----------------------------------------------------------------------------------------------------------------------
    ///@brief needed for setup for physics world
    //----------------------------------------------------------------------------------------------------------------------
    btDefaultCollisionConfiguration* m_collisionConfiguration;
    btCollisionDispatcher* m_dispatcher;
    btBroadphaseInterface* m_overlappingPairCache ;
    btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;
    btCollisionShape* m_groundShape;
    std::vector <Body> m_bodies;
};

#endif

