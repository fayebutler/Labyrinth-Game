//----------------------------------------------------------------------------------------------------------------------
/// @file CollisionShape.cpp
/// @brief simple class to store instances of bullet collision
/// shapes this is a singleton so we only have one instance of
/// each shape.
//----------------------------------------------------------------------------------------------------------------------

#include "CollisionShape.h"
#include <ngl/Obj.h>

//----------------------------------------------------------------------------------------------------------------------

CollisionShape *CollisionShape::s_instance=0;

CollisionShape *CollisionShape::instance()
{

  if(s_instance ==0 )
  {
    s_instance = new CollisionShape;
  }
  return s_instance;
}

//----------------------------------------------------------------------------------------------------------------------

void CollisionShape::addSphere(const std::string & _name, const std::string &_objFilePath)
{
  ngl::Obj mesh(_objFilePath);

	btConvexHullShape*  shape =  new btConvexHullShape();
	unsigned int n=mesh.getNumVerts();
	std::vector <ngl::Vec3> points=mesh.getVertexList();
	for(unsigned int i=0; i<n; ++i)
	{
		shape->addPoint(btVector3(points[i].m_x,points[i].m_y,points[i].m_z));
	}
	m_shapes[_name]=shape;
}

//----------------------------------------------------------------------------------------------------------------------

void CollisionShape::addMaze(const std::string & _name, const std::string &_objFilePath)
{
  ngl::Obj mesh(_objFilePath);
  //mesh has holes so use btbvhtrianglemesh
	btTriangleMesh * data= new btTriangleMesh();

	int numTris = mesh.getNumFaces();
	for(int i=0; i<numTris; ++i)
	{
		ngl::Face face = mesh.getFaceList()[i];
		unsigned int index0 = face.m_vert[0];
		unsigned int index1 = face.m_vert[1];
		unsigned int index2 = face.m_vert[2];
		ngl::Vec3 p0 = mesh.getVertexAtIndex(index0);
		ngl::Vec3 p1 = mesh.getVertexAtIndex(index1);
		ngl::Vec3 p2 = mesh.getVertexAtIndex(index2);

		btVector3 v0(p0[0], p0[1], p0[2]);
		btVector3 v1(p1[0], p1[1], p1[2]);
		btVector3 v2(p2[0], p2[1], p2[2]);

		data->addTriangle(v0, v1, v2, false);
	}

	btBvhTriangleMeshShape *shape = new btBvhTriangleMeshShape(data, true, true);
	m_shapes[_name]=shape;
}

//----------------------------------------------------------------------------------------------------------------------

btCollisionShape* CollisionShape::getShape(const std::string &_name)
{
	btCollisionShape *shape=0;
	std::map <std::string, btCollisionShape * >::const_iterator shapeIt=m_shapes.find(_name);

	if(shapeIt!=m_shapes.end())
	{
		shape=shapeIt->second;
	}
	return shape;
}

//----------------------------------------------------------------------------------------------------------------------

void CollisionShape::addBox(const std::string & _name, const std::string &_objFilePath)
{
  ngl::Obj mesh(_objFilePath);

	btConvexHullShape*  shape =  new btConvexHullShape();
	unsigned int n=mesh.getNumVerts();
	std::vector <ngl::Vec3> points=mesh.getVertexList();
	for(unsigned int i=0; i<n; ++i)
	{
		shape->addPoint(btVector3(points[i].m_x,points[i].m_y,points[i].m_z));
	}
	m_shapes[_name]=shape;
}


