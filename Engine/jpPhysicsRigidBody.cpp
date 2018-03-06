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

void jpPhysicsRigidBody::ToStatic()
{
	if (is_dynamic)
	{
		physx::PxScene*	scene = body->getScene();

		if (scene)
		{
			physx::PxRigidActor* temp_body = physx::PxCreateStatic(scene->getPhysics(), body->getGlobalPose(), body_shape->getGeometry().any(), *default_material);
			body->release();

			body = temp_body;
			body->getShapes(&body_shape, 1);
			scene->addActor(*temp_body);
		}
		is_dynamic = false;
	}
}

void jpPhysicsRigidBody::ToDynamic()
{
	if (!is_dynamic)
	{
		physx::PxScene*	scene = body->getScene();
		if (scene)
		{
			physx::PxRigidActor* temp_body = physx::PxCreateDynamic(scene->getPhysics(), body->getGlobalPose(), body_shape->getGeometry().any(), *default_material, 1.0f);
			body->release();

			body = temp_body;
			body->getShapes(&body_shape, 1);
			scene->addActor(*temp_body);
		}
		is_dynamic = true;
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

void jpPhysicsRigidBody::SetAsTrigger(bool trigger)
{
	if (body_shape)
	{
		if (trigger)
		{
			body_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			body_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
		}
		else
		{
			body_shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
			body_shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
		}
	}
}

void jpPhysicsRigidBody::SetSleepTime(float time)
{
	if (body && is_dynamic && time > 0)
	{
		static_cast<physx::PxRigidDynamic*> (body)->setWakeCounter(time);
	}
}

void jpPhysicsRigidBody::SetFilterFlags(unsigned int own_flag, unsigned int collision_flags)
{
	if (body_shape)
	{
		physx::PxFilterData filterData;
		filterData.word0 = own_flag; // word0 = own ID
		filterData.word1 = collision_flags;	// word1 = ID mask to filter pairs that trigger a contact callback;
		body_shape->setSimulationFilterData(filterData);
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
	if (body)
	{
		body->detachShape(*body_shape);
		body_shape = body->createShape(new_geometry, *default_material);
		body->attachShape(*body_shape);
	}
}

void jpPhysicsRigidBody::SetMaterial(float &static_friction, float &dynamic_friction, float &restitution)
{
	if (default_material)
	{
		if (static_friction < 0) static_friction = 0;
		else if (static_friction > PX_MAX_F32) static_friction = PX_MAX_F32 - 1;

		if (dynamic_friction < 0) dynamic_friction = 0;
		else if (dynamic_friction > PX_MAX_F32) dynamic_friction = PX_MAX_F32 - 1;

		if (restitution < 0) restitution = 0;
		else if (restitution > 1) restitution = 1;

		default_material->setStaticFriction(static_friction);
		default_material->setDynamicFriction(dynamic_friction);
		default_material->setRestitution(restitution);
	}
}

void jpPhysicsRigidBody::SetShape(physx::PxShape * new_shape)
{
	//Might not work
	if (body_shape)
	{
		body->detachShape(*body_shape);
	}
		
	if (new_shape)
	{
		body_shape = new_shape;
		body->attachShape(*body_shape);
	}
}

void jpPhysicsRigidBody::SetGeometry(float3 new_scale, float radius, JP_COLLIDER_TYPE type)
{
	if (!new_scale.IsFinite())
	{
		return;
	}

	physx::PxVec3 scale = physx::PxVec3(new_scale.x, new_scale.y, new_scale.z);
	
	if (radius < 0)
		radius = -radius;
	if(scale.isFinite())
		scale = scale.abs();
	else scale = physx::PxVec3(0.0, 0.0, 0.0);
		
	if (body_shape) 
	{
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

void jpPhysicsRigidBody::SetShapeScale(float3& new_scale, float& radius, JP_COLLIDER_TYPE shape_type)
{
	if (body_shape) {
		physx::PxVec3 scale = physx::PxVec3(new_scale.x, new_scale.y, new_scale.z);

		// Get absolute values for scale and radius
		if (radius < 0)
			radius = -radius;

		if (scale.isFinite())
		{
			scale = scale.abs();
			new_scale = float3(scale.x, scale.y, scale.z);
		}
		else
		{
			scale = physx::PxVec3(0.0, 0.0, 0.0);
		}

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
		default:
			break;
		}
	}
}

void jpPhysicsRigidBody::SetMass(float & mass)
{
	if (is_dynamic)
	{
		if (mass < 0)
		{
			mass = 0;
		}
		if (mass > PX_MAX_F32)
		{
			mass = PX_MAX_F32 - 1;
		}

		static_cast<physx::PxRigidDynamic*> (body)->setMass(mass);
	}
}

void jpPhysicsRigidBody::SetDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum flag, bool active)
{
	if (body && is_dynamic)
	{
		static_cast<physx::PxRigidDynamic*>(body)->setRigidDynamicLockFlag(flag,active);
	}
}

void jpPhysicsRigidBody::SetDynamicLock()
{
	if (body && is_dynamic)
	{
		physx::PxRigidDynamic* rbody = (physx::PxRigidDynamic*) body;
		rbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_X, true);
		rbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Y, true);
		rbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::Enum::eLOCK_LINEAR_Z, true);
	}
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
	if (body)
	{
		return body;
	}
	else return nullptr;
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

void jpPhysicsRigidBody::ApplyForce(float3 force)
{
	if (is_dynamic && force.IsFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->addForce(physx::PxVec3(force.x, force.y, force.z));
	}
}

void jpPhysicsRigidBody::ApplyImpulse(float3 impulse)
{
	if (is_dynamic && impulse.IsFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->addForce(physx::PxVec3(impulse.x, impulse.y, impulse.z), physx::PxForceMode::Enum::eIMPULSE);
	}
}

void jpPhysicsRigidBody::ApplyTorqueForce(float3 force)
{
	if (is_dynamic && force.IsFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->addTorque(physx::PxVec3(force.x, force.y, force.z));
	}
}

void jpPhysicsRigidBody::ApplyTorqueImpulse(float3 impulse)
{
	if (is_dynamic && impulse.IsFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->addTorque(physx::PxVec3(impulse.x, impulse.y, impulse.z), physx::PxForceMode::Enum::eIMPULSE);
	}
}

void jpPhysicsRigidBody::MoveKinematic(float3 pos, Quat rotation)
{
	if (is_dynamic && pos.IsFinite() && rotation.IsFinite())
	{
		static_cast<physx::PxRigidDynamic*> (body)->setKinematicTarget(physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)));
	}
}

void jpPhysicsRigidBody::ResetForces()
{
	if (body && is_dynamic)
	{
		static_cast<physx::PxRigidBody*>(body)->setLinearVelocity(physx::PxVec3(0,0,0));
		static_cast<physx::PxRigidBody*>(body)->setAngularVelocity(physx::PxVec3(0,0,0));
	}
}
