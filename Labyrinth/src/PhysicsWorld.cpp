//----------------------------------------------------------------------------------------------------------------------
/// @file PhysicsWorld.cpp
/// @brief set physics for objects and physics world
//----------------------------------------------------------------------------------------------------------------------

#include "PhysicsWorld.h"
#include "CollisionShape.h"
#include <ngl/Obj.h>
#include "NGLDraw.h"

//----------------------------------------------------------------------------------------------------------------------

PhysicsWorld::PhysicsWorld()
{
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	m_collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	m_overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_solver = new btSequentialImpulseConstraintSolver;

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher,m_overlappingPairCache,m_solver,m_collisionConfiguration);

	m_dynamicsWorld->getSolverInfo().m_solverMode = SOLVER_USE_WARMSTARTING + SOLVER_SIMD;


}

//----------------------------------------------------------------------------------------------------------------------

void PhysicsWorld::addSphere(std::string _shapeName,const ngl::Vec3 &_pos, float _friction)
{

	//create a dynamic rigidbody
	btCollisionShape* fallshape = CollisionShape::instance()->getShape(_shapeName);

	btTransform startTransform;
	startTransform.setIdentity();
	btScalar mass = 1;
	btVector3 fallInertia(0,0,0);
	fallshape->calculateLocalInertia(mass,fallInertia);
	startTransform.setOrigin(btVector3(_pos.m_x, _pos.m_y, _pos.m_z));
	btDefaultMotionState * fallMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);

	btRigidBody * fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setFriction(_friction);
	//set collision flag for contact test call back
	fallRigidBody->setCollisionFlags(fallRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_dynamicsWorld->addRigidBody(fallRigidBody);
	Body b;
	b.name= _shapeName;
	b.body =fallRigidBody;
	m_bodies.push_back(b);

}

//----------------------------------------------------------------------------------------------------------------------

void PhysicsWorld::addGroundPlane(const ngl::Vec3 &_pos, const ngl::Vec3 &_size)
{
	m_groundShape = new btStaticPlaneShape(btVector3(0,1,0),_pos.m_y);

	btTransform groundTransform;
	groundTransform.setIdentity();
	{
			btScalar mass(0.);
			btVector3 localInertia(0,0,0);

			//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,m_groundShape,localInertia);

			btRigidBody* body = new btRigidBody(rbInfo);
			body->setFriction(1.);
			body->setRollingFriction(2.);
			//add the body to the dynamics world
			m_dynamicsWorld->addRigidBody(body);
			Body b;
			b.name="groundPlane";
			b.body=body;
			m_bodies.push_back(b);

		}

}

//----------------------------------------------------------------------------------------------------------------------

PhysicsWorld::~PhysicsWorld()
{
	//delete dynamics world
		delete m_dynamicsWorld;

		//delete solver
		delete m_solver;

		//delete broadphase
		delete m_overlappingPairCache;

		//delete dispatcher
		delete m_dispatcher;

		delete m_collisionConfiguration;

}

//----------------------------------------------------------------------------------------------------------------------

void PhysicsWorld::addMaze(std::string _shapeName,const ngl::Vec3 &_pos, float _friction)
{

	btCollisionShape* colShape = CollisionShape::instance()->getShape(_shapeName);
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar mass(0.0);

	btVector3 localInertia(10,10,10);
	colShape->calculateLocalInertia(mass,localInertia);
	startTransform.setOrigin(btVector3(_pos.m_x,_pos.m_y,_pos.m_z));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	m_dynamicsWorld->addRigidBody(body);
	//set friction from config file
	//divide by 10 as want maze to have low friction for calculation
	body->setFriction(_friction/10);
	//set as kinematic object
	body->setCollisionFlags(body->getCollisionFlags()|btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);
	Body b;
	b.name=_shapeName;
	b.body=body;
	m_bodies.push_back(b);
}

//----------------------------------------------------------------------------------------------------------------------

void PhysicsWorld::setRot(unsigned int _index, btQuaternion _rot)
{
	btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[_index];
	btRigidBody* body = btRigidBody::upcast(obj);
	if (body && body->getMotionState())
	{
		//offset to origin for rotation then move back
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		btQuaternion rot = _rot;
		btVector3 offset = trans.getOrigin();
		trans.setOrigin(btVector3(0,0,0));
		trans.setRotation(rot);
		trans.setOrigin(offset);
		body->getMotionState()->setWorldTransform(trans);
	}
}

//----------------------------------------------------------------------------------------------------------------------

void PhysicsWorld::setRotAboutOrigin(unsigned int _index, btQuaternion _rot)
{
	btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[_index];
	btRigidBody* body = btRigidBody::upcast(obj);
	if (body && body->getMotionState())
	{
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		btQuaternion rot = _rot;
		trans.setRotation(rot);
		body->getMotionState()->setWorldTransform(trans);
	}
}

//----------------------------------------------------------------------------------------------------------------------

void PhysicsWorld::step(float _time, float _step)
{
  m_dynamicsWorld->stepSimulation(_time,_step);
}

//----------------------------------------------------------------------------------------------------------------------

int PhysicsWorld::getCollisionShape(unsigned int _index) const
{
  btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[_index];
  btCollisionShape *collisionShape = obj->getCollisionShape();

  return collisionShape->getShapeType();
}

//----------------------------------------------------------------------------------------------------------------------

void * PhysicsWorld::getUserData(unsigned int _index)
{
  btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[_index];
  btCollisionShape *collisionShape = obj->getCollisionShape();
  return collisionShape->getUserPointer();
}

//----------------------------------------------------------------------------------------------------------------------

ngl::Mat4 PhysicsWorld::getTransformMatrix(unsigned int _index)
{
	btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[_index];
	btRigidBody* body = btRigidBody::upcast(obj);
	if (body && body->getMotionState())
	{
		btTransform trans;

		body->getMotionState()->getWorldTransform(trans);
		float matrix[16];
		trans.getOpenGLMatrix(matrix);
		return ngl::Mat4( matrix[0],matrix[1],matrix[2],matrix[3],
											 matrix[4],matrix[5],matrix[6],matrix[7],
											 matrix[8],matrix[9],matrix[10],matrix[11],
											 matrix[12],matrix[13],matrix[14],matrix[15]
				);
	}

}

//----------------------------------------------------------------------------------------------------------------------

btQuaternion PhysicsWorld::getRotation(unsigned int _index)
{
	btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[_index];
	btRigidBody* body = btRigidBody::upcast(obj);
	if (body && body->getMotionState())
	{
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		return trans.getRotation();
	}
}

//----------------------------------------------------------------------------------------------------------------------

ngl::Vec3 PhysicsWorld::getPosition(unsigned int _index)
{
	btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[_index];
	btRigidBody* body = btRigidBody::upcast(obj);
	if (body && body->getMotionState())
	{
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		return ngl::Vec3(trans.getOrigin().getX(),
										 trans.getOrigin().getY(),
										 trans.getOrigin().getZ()
										);
	}
}

//----------------------------------------------------------------------------------------------------------------------

bool collision = false;

void PhysicsWorld::removeBody(unsigned int _index)
{
	m_dynamicsWorld->removeRigidBody(m_bodies[_index+1].body);
}

//----------------------------------------------------------------------------------------------------------------------

void PhysicsWorld::reset()
{
	// start at 1 to leave the ground plane
	for(unsigned int i=1; i<m_bodies.size(); ++i)
	{
		m_dynamicsWorld->removeRigidBody(m_bodies[i].body);

	}
	m_bodies.erase(m_bodies.begin()+1,m_bodies.end());
	//reset collision
	collision=false;

}

//----------------------------------------------------------------------------------------------------------------------

struct ContactSensorCallBack : public btDynamicsWorld::ContactResultCallback
{
	ContactSensorCallBack(btCollisionObject& cube, btCollisionObject& ball) : btDynamicsWorld::ContactResultCallback(), cube(cube), ball(ball) {}

	btCollisionObject& cube;
	btCollisionObject& ball;

	virtual btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0, int partId0, int index0, const btCollisionObjectWrapper* colObj1, int partId1, int index1)
	{
		btVector3 pt;
		if(colObj0->m_collisionObject==&cube && colObj1->m_collisionObject==&ball)
		{
			//make sure collision between cube and ball
			//set collision to true
			collision=true;
		}
		return 0;
	}
};

//----------------------------------------------------------------------------------------------------------------------

bool PhysicsWorld::contactTest(unsigned int i, unsigned int j)
{
	btCollisionObject* cube;
	btCollisionObject* ball;
	collision=false;

	cube = m_dynamicsWorld->getCollisionObjectArray()[j];
	ball = m_dynamicsWorld->getCollisionObjectArray()[i];

	ContactSensorCallBack callback(*cube, *ball);
	m_dynamicsWorld->contactPairTest(cube, ball, callback);

	return collision;
}

//----------------------------------------------------------------------------------------------------------------------

void PhysicsWorld::addCube(std::string _shapeName, const ngl::Vec3 &_pos)
{
	btCollisionShape* colShape = CollisionShape::instance()->getShape(_shapeName);

	// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	btScalar	mass(0);

	btVector3 localInertia(10,10,10);
	colShape->calculateLocalInertia(mass,localInertia);
	startTransform.setOrigin(btVector3(_pos.m_x,_pos.m_y,_pos.m_z));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	rbInfo.m_restitution = 0;
	rbInfo.m_friction = 100.5f;
	rbInfo.m_additionalAngularDampingFactor=4.0;
	rbInfo.m_additionalDamping=true;
	btRigidBody* body = new btRigidBody(rbInfo);
	//set as kinematic object
	//set custom callback
	body->setCollisionFlags(body->getCollisionFlags() |btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	body->setCollisionFlags(body->getCollisionFlags()|btCollisionObject::CF_KINEMATIC_OBJECT);
	body->setActivationState(DISABLE_DEACTIVATION);
	m_dynamicsWorld->addRigidBody(body);
	Body b;
	b.name=_shapeName;
	b.body=body;
	m_bodies.push_back(b);
}

//----------------------------------------------------------------------------------------------------------------------

