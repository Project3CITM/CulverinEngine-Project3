#include "jpPhysicsRigidBody.h"


jpPhysicsRigidBody::jpPhysicsRigidBody(physx::PxPhysics* px_physics, bool is_dynamic) : is_dynamic(is_dynamic)
{
	//Create a default material 
	default_material = px_physics->createMaterial(0.5f, 0.5f, 0.0f);

	if (is_dynamic)
	{
		//Create RigidDynamic with default properties
		body = physx::PxCreateDynamic(*px_physics, physx::PxTransform(physx::PxIDENTITY()), physx::PxBoxGeometry(0.5f, 0.5f, 0.5f), *default_material, 1.0f);
		
		//Default RigidDynamics don't have shape so we remove it after creation time
		body->getShapes(&body_shape, 1);
		body->detachShape(*body_shape);
		body_shape = nullptr;
	}
	else
	{
		//Create RigidStatic with default properties
		body = physx::PxCreateStatic(*px_physics, physx::PxTransform(physx::PxIDENTITY()), physx::PxBoxGeometry(0.5,0.5,0.5), *default_material);
		body->getShapes(&body_shape, 1);
	}
	
	
}

jpPhysicsRigidBody::~jpPhysicsRigidBody()
{
	if (body)
	{
		body->release();
	}
}

void jpPhysicsRigidBody::ActivateShape()
{
	if (body_shape && body)
	{
		body->attachShape(*body_shape);
	}
}

void jpPhysicsRigidBody::DeActivateShape()
{
	if (body_shape && body)
	{
		body->detachShape(*body_shape);
	}
	
	body_shape = nullptr;
}

void jpPhysicsRigidBody::SetAsKinematic(bool kinematic)
{
	if (is_dynamic)
	{
		if (kinematic)
		{
			static_cast<physx::PxRigidDynamic*> (body)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
		}
		else
		{
			static_cast<physx::PxRigidDynamic*> (body)->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);
		}
	}
}

void jpPhysicsRigidBody::SetTransform(float * trans_mat)
{
	physx::PxMat44 mat = physx::PxMat44(trans_mat);

	if (body)
	{
		body->setGlobalPose(physx::PxTransform(mat));
	}
}

void jpPhysicsRigidBody::SetTransform(float3 pos, Quat rotation, bool autoawake)
{
	if (body && pos.IsFinite() && rotation.IsFinite())
	{
		body->setGlobalPose(physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)), autoawake);
	}
}

void jpPhysicsRigidBody::SetGeometry(physx::PxGeometry new_geometry)
{
	//Get the current material for the new shape
//	physx::PxMaterial* body_material = nullptr;
//	body_shape->getMaterials(&body_material, 1);

	body->detachShape(*body_shape);
	body_shape = body->createShape(new_geometry, *default_material);
	body->attachShape(*body_shape);
}

void jpPhysicsRigidBody::SetMaterial(float &static_friction, float &dynamic_friction, float &restitution)
{
	if (static_friction < 0) static_friction = 0;
	else if (static_friction > PX_MAX_F32) static_friction = PX_MAX_F32-1;

	if (dynamic_friction < 0) dynamic_friction = 0;
	else if (dynamic_friction > PX_MAX_F32) dynamic_friction = PX_MAX_F32-1;

	if (restitution < 0) restitution = 0;
	else if (restitution > 1) restitution = 1;

	//physx::PxMaterial* material;
	//body_shape->getMaterials(&material, 1);
	default_material->setStaticFriction(static_friction);
	default_material->setDynamicFriction(dynamic_friction);
	default_material->setRestitution(restitution);
}

void jpPhysicsRigidBody::SetShape(physx::PxShape * new_shape)
{
	//Might not work
	if (body_shape)
	{
		body->detachShape(*body_shape);
	}
	
	body_shape = new_shape;
	
	if (body_shape)
	{
		body->attachShape(*body_shape);
	}
}

void jpPhysicsRigidBody::SetGeometry(physx::PxVec3 scale, float radius, JP_COLLIDER_TYPE type)
{
	if (radius < 0)
		radius = -radius;
	if(scale.isFinite())
		scale = scale.abs();
	else scale = physx::PxVec3(0.0, 0.0, 0.0);
		
	if (body_shape) {
		body->detachShape(*body_shape);
		body_shape = nullptr;
	}
	
	switch (type)
	{
	case COLL_SPHERE:
		body_shape = body->createShape(physx::PxSphereGeometry(radius), *default_material);
		break;
	case COLL_PLANE:{
		scale = scale*0.5;
		body_shape = body->createShape(physx::PxBoxGeometry(scale.x, physx::PxReal(0.01), scale.z), *default_material);
	}
		break;
	case COLL_CAPSULE:
		body_shape = body->createShape(physx::PxCapsuleGeometry(radius, scale.z), *default_material);
		break;
	case COLL_BOX: {
		scale = scale*0.5;
		body_shape = body->createShape(physx::PxBoxGeometry(scale.x, scale.y, scale.z), *default_material);
	}
		break;
	default: body_shape = body->createShape(physx::PxSphereGeometry(0.5), *default_material);
		break;
	}	

	if (body_shape) {
		body_shape->setContactOffset(0.02);
		body_shape->setRestOffset(0);
	}
}

void jpPhysicsRigidBody::SetShapeScale(physx::PxVec3 scale, float radius, JP_COLLIDER_TYPE shape_type)
{
	if (body_shape) {
		// Get absolute values for scale and radius
		if (radius < 0)
			radius = -radius;

		if (scale.isFinite())
			scale = scale.abs();
		else scale = physx::PxVec3(0.0, 0.0, 0.0);

		// Modify Geometry Scale
		physx::PxGeometryType::Enum type = body_shape->getGeometryType();
		switch (type)
		{
		case physx::PxGeometryType::eSPHERE: {
			physx::PxSphereGeometry sphere = body_shape->getGeometry().sphere();
			sphere.radius = radius;
			body_shape->setGeometry(sphere);
		}
		break;
		case physx::PxGeometryType::eCAPSULE: {
			physx::PxCapsuleGeometry capsule = body_shape->getGeometry().capsule();
			capsule.halfHeight = scale.z*0.5f;
			capsule.radius = radius;
			body_shape->setGeometry(capsule);
		}
		break;
		case physx::PxGeometryType::eBOX:{
			physx::PxBoxGeometry box = body_shape->getGeometry().box();
			if(shape_type == COLL_BOX)
				box.halfExtents = scale*0.5;
			else {
				scale.y = 0.02f;
				box.halfExtents = scale*0.5;
			}
			body_shape->setGeometry(box);
		}
		break;
		case physx::PxGeometryType::eCONVEXMESH: {
			physx::PxConvexMeshGeometry convmesh = body_shape->getGeometry().convexMesh();
			convmesh.scale = scale;
			body_shape->setGeometry(convmesh);
		}
			break;
		case physx::PxGeometryType::eTRIANGLEMESH: {
			physx::PxTriangleMeshGeometry trimesh = body_shape->getGeometry().triangleMesh();
			trimesh.scale = scale;
			body_shape->setGeometry(trimesh);
		}
			break;
		case physx::PxGeometryType::eHEIGHTFIELD: {
			physx::PxHeightFieldGeometry heightfield = body_shape->getGeometry().heightField();
			heightfield.heightScale = scale.z;
			heightfield.rowScale = scale.x;
			heightfield.columnScale = scale.y;
			body_shape->setGeometry(heightfield);
		}
			break;
		default:
			break;
		}
	}
}

void jpPhysicsRigidBody::SetMass(float & mass)
{
	if (mass < 0)
		mass = 0;
	if (mass > PX_MAX_F32)
		mass = PX_MAX_F32 - 1;
	
	if (is_dynamic)
		static_cast<physx::PxRigidDynamic*> (body)->setMass(mass);
}

void jpPhysicsRigidBody::GetTransform(float3 & pos, Quat & rotation)
{
	if (is_dynamic)
	{
		physx::PxTransform transf = static_cast<physx::PxRigidDynamic*> (body)->getGlobalPose();
		pos = float3(transf.p.x, transf.p.y, transf.p.z);
		rotation = Quat(transf.q.x, transf.q.y, transf.q.z, transf.q.w);
	}
	else
	{
		pos = float3::zero;
		rotation = Quat::identity;
	}
}

physx::PxRigidActor * jpPhysicsRigidBody::GetActor()
{
	if(body)
		return body;
}

bool jpPhysicsRigidBody::Sleeping()
{
	if (is_dynamic && ((physx::PxRigidDynamic*)body)->isSleeping())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void jpPhysicsRigidBody::ApplyForce(physx::PxVec3 force)
{
	if (is_dynamic && force.isFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->addForce(force);
	}
}

void jpPhysicsRigidBody::ApplyImpulse(physx::PxVec3 impulse)
{
	if (is_dynamic && impulse.isFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->addForce(impulse, physx::PxForceMode::Enum::eIMPULSE);
	}
}

void jpPhysicsRigidBody::ApplyTorqueForce(physx::PxVec3 force)
{
	if (is_dynamic && force.isFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->addTorque(force);
	}
}

void jpPhysicsRigidBody::ApplyTorqueImpulse(physx::PxVec3 impulse)
{
	if (is_dynamic && impulse.isFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->addTorque(impulse, physx::PxForceMode::Enum::eIMPULSE);
	}
}

void jpPhysicsRigidBody::MoveKinematic(physx::PxTransform dest)
{
	if (is_dynamic)
	{
		static_cast<physx::PxRigidDynamic*> (body)->setKinematicTarget(dest);
	}
}
