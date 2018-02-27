#include <algorithm>
#include "Timer.h"
#include "ParticleSystem.h"
#include "GL3W\include\glew.h"
#include "ImGui\imgui.h"
#include "ModuleEventSystem.h"

float ParticleSystem_Lerp(float v0, float v1, float time, float Maxtime)
{
	float Time = time / Maxtime;
	return (1.0f - Time) * v0 + Time * v1;
}

float3 ParticleSystem_Lerp(float3 v0, float3 v1, float time, float Maxtime)
{
	float Time = time / Maxtime;
	return (1.0f - Time) * v0 + Time * v1;
}

float4 ParticleSystem_Lerp(float4 v0, float4 v1, float time, float Maxtime)
{
	float Time = time / Maxtime;
	return (1.0f - Time) * v0 + Time * v1;
}

ParticleEmitter::ParticleEmitter()
{
	BoundingBox.minPoint = float3(-0.5f, -0.5f, -0.5f);
	BoundingBox.maxPoint = float3( 0.5f,  0.5f,  0.5f);
	ResetEmitterValues();
}

ParticleEmitter::~ParticleEmitter()
{

}

void ParticleEmitter::DebugDrawEmitter()
{
	glPushMatrix();
	glMultMatrixf(Transform.ptr());
	switch (Type)
	{
	case EmitterType_Sphere: DrawSphere(EmitterShape.Sphere_Shape); break;
	case EmitterType_SemiSphere: DrawSemiSphere(EmitterShape.Sphere_Shape); break;
	case EmitterType_Cone: DrawCone(EmitterShape.ConeTrunk_Shape); break;
	case EmitterType_Box: DrawBox(EmitterShape.Box_Shape); break;
	case EmitterType_Circle: DrawCircle(EmitterShape.Circle_Shape); break;
	}
	glPopMatrix();
}

void ParticleEmitter::DebugDrawEmitterAABB()
{
	glPushMatrix();
	glMultMatrixf(Transform.ptr());
	DrawBox(BoundingBox);
	glPopMatrix();
}

void ParticleEmitter::ResetEmitterValues()
{
	switch (Type)
	{
	case EmitterType_Sphere:
	case EmitterType_SemiSphere:
		EmitterShape.Sphere_Shape.pos = float3::zero;
		EmitterShape.Sphere_Shape.r = 1.0f;
		break;
	case EmitterType_Cone:
		EmitterShape.ConeTrunk_Shape.Upper_Circle.r = 1.5f;
		EmitterShape.ConeTrunk_Shape.Upper_Circle.pos = float3::zero;
		EmitterShape.ConeTrunk_Shape.Upper_Circle.normal = float3(0.0f, 1.0f, 0.0f);
		EmitterShape.ConeTrunk_Shape.Bottom_Circle.r = 1.0f;
		EmitterShape.ConeTrunk_Shape.Bottom_Circle.pos = float3::zero;
		EmitterShape.ConeTrunk_Shape.Bottom_Circle.normal = float3(0.0f, 1.0f, 0.0f);
		EmitterShape.ConeTrunk_Shape.heigth = 1.0f;
		break;
	case EmitterType_Box:
		EmitterShape.Box_Shape.maxPoint = float3(0.5f, 0.5f, 0.5f);
		EmitterShape.Box_Shape.minPoint = float3(-0.5f, -0.5f, -0.5f);
		break;
	case EmitterType_Circle:
		EmitterShape.Circle_Shape.r = 1.0f;
		EmitterShape.Circle_Shape.pos = float3::zero;
		EmitterShape.Circle_Shape.normal = float3(0.0f, 1.0f, 0.0f);
		break;
	}
}

void ParticleEmitter::SetTransform(const float4x4& transform)
{
	Transform = transform;
}

void ParticleEmitter::GetPosition(float3 & pos)
{
	Quat rotation = Quat::identity;
	float3 scale = float3::zero;
	Transform.Transposed().Decompose(pos, rotation, scale);
}

void ParticleEmitter::GetRotation(Quat & rot)
{
	float3 position = float3::zero;
	float3 scale = float3::zero;
	Transform.Transposed().Decompose(position, rot, scale);
}

void ParticleEmitter::GetScale(float3 & sca)
{
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	Transform.Transposed().Decompose(position, rotation, sca);
}

void ParticleEmitter::DrawSphere(const Sphere& shape)
{
	glLineWidth(DEBUG_THICKNESS);
	glColor3f(DEBUG_COLOR_R, DEBUG_COLOR_G, DEBUG_COLOR_B);
	float radius = shape.r;
	float degInRad = 360.0f / CIRCLES_SEGMENTS;
	degInRad *= DEGTORAD;
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < CIRCLES_SEGMENTS; i++)
		glVertex3f(cos(degInRad * i) * radius, 0.0f, sin(degInRad * i) * radius);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < CIRCLES_SEGMENTS; i++)
		glVertex3f(cos(degInRad * i) * radius, sin(degInRad * i) * radius, 0.0f);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < CIRCLES_SEGMENTS; i++)
		glVertex3f(0.0f, sin(degInRad * i) * radius, cos(degInRad * i) * radius);
	glEnd();
	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void ParticleEmitter::DrawSemiSphere(const Sphere& shape)
{
	glLineWidth(DEBUG_THICKNESS);
	glColor3f(DEBUG_COLOR_R, DEBUG_COLOR_G, DEBUG_COLOR_B);
	float radius = shape.r;
	float degInRad = 360.0f / CIRCLES_SEGMENTS;
	degInRad *= DEGTORAD;
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < CIRCLES_SEGMENTS; i++)
		glVertex3f(cos(degInRad * i) * radius, 0.0f, sin(degInRad * i) * radius);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i <= CIRCLES_SEGMENTS * 0.5f; i++)
		glVertex3f(cos(degInRad * i) * radius, sin(degInRad * i) * radius, 0.0f);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i <= CIRCLES_SEGMENTS * 0.5f; i++)
		glVertex3f(0.0f, sin(degInRad * i) * radius, cos(degInRad * i) * radius);
	glEnd();
	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void ParticleEmitter::DrawCone(const ConeTrunk& shape)
{
	glLineWidth(DEBUG_THICKNESS);
	glColor3f(DEBUG_COLOR_R, DEBUG_COLOR_G, DEBUG_COLOR_B);
	float Height = shape.heigth;
	float radius = shape.Bottom_Circle.r;
	float radius_top = shape.Upper_Circle.r;
	float degInRad = 360.0f / CIRCLES_SEGMENTS;
	degInRad *= DEGTORAD;
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < CIRCLES_SEGMENTS; i++)
		glVertex3f(cos(degInRad * i) * radius, 0.0f, sin(degInRad * i) * radius);
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i <= CIRCLES_SEGMENTS; i++)
		glVertex3f(cos(degInRad * i) * radius_top, Height, sin(degInRad * i) * radius_top);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f(radius, 0.0f, 0.0f);
	glVertex3f(radius_top, Height, 0.0f);
	glVertex3f(-radius_top, Height, 0.0f);
	glVertex3f(-radius, 0.0f, 0.0f);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0f, 0.0f, -radius);
	glVertex3f(0.0f, Height, -radius_top);
	glVertex3f(0.0f, Height, radius_top);
	glVertex3f(0.0f, 0.0f, radius);
	glEnd();
	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void ParticleEmitter::DrawBox(const AABB& shape)
{
	float3 p0 = shape.CornerPoint(0);
	float3 p1 = shape.CornerPoint(1);
	float3 p2 = shape.CornerPoint(2);
	float3 p3 = shape.CornerPoint(3);
	float3 p4 = shape.CornerPoint(4);
	float3 p5 = shape.CornerPoint(5);
	float3 p6 = shape.CornerPoint(6);
	float3 p7 = shape.CornerPoint(7);

	glLineWidth(DEBUG_THICKNESS);
	glColor3f(DEBUG_COLOR_R, DEBUG_COLOR_G, DEBUG_COLOR_B);

	glBegin(GL_LINES);

	glVertex3f(p0.x, p0.y, p0.z);
	glVertex3f(p1.x, p1.y, p1.z);

	glVertex3f(p0.x, p0.y, p0.z);
	glVertex3f(p2.x, p2.y, p2.z);

	glVertex3f(p0.x, p0.y, p0.z);
	glVertex3f(p4.x, p4.y, p4.z);

	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p6.x, p6.y, p6.z);

	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p3.x, p3.y, p3.z);

	glVertex3f(p7.x, p7.y, p7.z);
	glVertex3f(p5.x, p5.y, p5.z);

	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p1.x, p1.y, p1.z);

	glVertex3f(p5.x, p5.y, p5.z);
	glVertex3f(p4.x, p4.y, p4.z);

	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p3.x, p3.y, p3.z);

	glVertex3f(p2.x, p2.y, p2.z);
	glVertex3f(p6.x, p6.y, p6.z);

	glVertex3f(p6.x, p6.y, p6.z);
	glVertex3f(p4.x, p4.y, p4.z);

	glVertex3f(p3.x, p3.y, p3.z);
	glVertex3f(p1.x, p1.y, p1.z);

	glEnd();

	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void ParticleEmitter::DrawCircle(const Circle& shape)
{
	glLineWidth(DEBUG_THICKNESS);
	glColor3f(DEBUG_COLOR_R, DEBUG_COLOR_G, DEBUG_COLOR_B);
	float radius = shape.r;
	float degInRad = 360.0f / (float)CIRCLES_SEGMENTS;
	degInRad *= DEGTORAD;
	glBegin(GL_LINE_LOOP);
	for (unsigned int i = 0; i < CIRCLES_SEGMENTS; i++)
		glVertex3f(cos(degInRad * i) * radius, 0.0f, sin(degInRad * i) * radius);
	glEnd();
	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

ParticleEmitter::EmitterShapeUnion::EmitterShapeUnion()
{

}

Particle::Particle(ParticleSystem* parent, const ParticleState& Initial, const ParticleState& Final, float3 Speed, float3 offset, float LifetimeMax) : ParentParticleSystem(parent)
{
	
	SetAssignedStateFromVariables(InitialState, Initial);
	SetAssignedStateFromVariables(FinalState, Final);
	Properties.Speed = Speed;
	Properties.LifetimeMax = LifetimeMax;
	parent->Emitter.GetPosition(Properties.Position);
	Properties.Position += offset;
}

Particle::~Particle()
{

}

bool Particle::PreUpdate(float dt)
{
	if (dt > 0.001f)
	{
		Properties.LifetimeActual += dt;
		CalculateStatesInterpolation();
	}
	return true;
}

bool Particle::Update(float dt)
{
	OrientateParticle();
	return true;
}

bool Particle::PostUpdate(float dt)
{
	if (!MeshChanged)
	{
		Event draw_event;
		draw_event.particle.type = EventType::EVENT_PARTICLE_DRAW;
		draw_event.particle.ToDraw = this;

		PushEvent(draw_event);
	}
	if (Properties.LifetimeActual >= Properties.LifetimeMax) ToDelete = true;

	return true;
}

bool Particle::isDead()
{
	return ToDelete;
}

void Particle::DrawParticle()
{
	const ParticleMeshData& Mesh = ParentParticleSystem->GetParticleMeshData();

	glColor4f(Properties.RGBATint.x, Properties.RGBATint.y, Properties.RGBATint.z, Properties.RGBATint.w);

	glDisable(GL_CULL_FACE);

	glEnable(GL_NORMALIZE);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	if (ParentParticleSystem->TextureData.TextureID != 0)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, ParentParticleSystem->TextureData.TextureID);
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, Mesh.id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (Mesh.normals != nullptr)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, Mesh.id_normals);
		glNormalPointer(GL_FLOAT, 0, NULL);
	}
	/*
	if (Mesh.texture_coords != nullptr) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, Mesh.id_texture_coords);
		glTexCoordPointer(3, GL_FLOAT, 0, NULL);
	}
	*/

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	unsigned int ID = ParentParticleSystem->GetTextureID(Properties.LifetimeMax, Properties.LifetimeActual);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glTexCoordPointer(3, GL_FLOAT, 0, NULL);

	glPushMatrix();
	float4x4 ParticleMatrix = float4x4::FromTRS(Properties.Position, Properties.Rotation, Properties.Scale * Properties.Size).Transposed();
	/*
	float4x4 ParticleMatrix = float4x4::identity;
	if (ParentParticleSystem->Emitter.EmissionType == 0)//LocalEmission
	{
		float4x4 gresg = float4x4::FromTRS(Properties.Position, Properties.Rotation, Properties.Scale * Properties.Size);
		ParticleMatrix = ParentParticleSystem->Emitter.Transform.Transposed() * gresg;
		//ParticleMatrix.Inverse();
		ParticleMatrix.Transpose();
		//ParticleMatrix = float4x4::FromTRS(Properties.Position, Properties.Rotation, Properties.Scale * Properties.Size) * ParentParticleSystem->Emitter.Transform;
		//ParticleMatrix.Transpose();
	}
	else
		ParticleMatrix = float4x4::FromTRS(Properties.Position, Properties.Rotation, Properties.Scale * Properties.Size).Transposed();
	*/
	glMultMatrixf(ParticleMatrix.ptr());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Mesh.id_indices);
	glDrawElements(GL_TRIANGLES, Mesh.num_indices, GL_UNSIGNED_INT, NULL);

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glDisable(GL_NORMALIZE);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void Particle::SetAssignedStateFromVariables(ParticleAssignedState& AState, const ParticleState& State)
{
	LCG RandGen;
	AState.force.x = State.force.x + RandGen.Float(-State.forceVariation.x, State.forceVariation.x);
	AState.force.y = State.force.y + RandGen.Float(-State.forceVariation.y, State.forceVariation.y);
	AState.force.z = State.force.z + RandGen.Float(-State.forceVariation.z, State.forceVariation.z);

	AState.Size = State.Size + RandGen.Float(-State.SizeVariation, State.SizeVariation);

	AState.RGBATint.x = State.RGBATint.x + RandGen.Float(-State.RGBATintVariation.x, State.RGBATintVariation.x);
	AState.RGBATint.y = State.RGBATint.y + RandGen.Float(-State.RGBATintVariation.y, State.RGBATintVariation.y);
	AState.RGBATint.z = State.RGBATint.z + RandGen.Float(-State.RGBATintVariation.z, State.RGBATintVariation.z);
	AState.RGBATint.w = State.RGBATint.w + RandGen.Float(-State.RGBATintVariation.w, State.RGBATintVariation.w);
}

void Particle::OrientateParticle()
{
	switch (ParentParticleSystem->Emitter.ParticleFacingOptions)
	{
	case 0: //Null
	{
		break;
	}
	case 1: //Billboard
	{
		float3 Direction = ParentParticleSystem->CameraPosition - Properties.Position;
		Direction.Normalize();
		Properties.Rotation = Quat::LookAt(float3(0.0f, 0.0f, 1.0f), Direction, float3(0.0f, 1.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
		break;
	}
	case 2: //VerticalBillboard
	{
		float3 Direction = ParentParticleSystem->CameraPosition - Properties.Position;
		Direction.y = Properties.Position.y;
		Properties.Rotation = Quat::LookAt(float3(0.0f, 0.0f, 1.0f), Direction, float3(0.0f, 1.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
		break;
	}
	case 3: //HorizontalBillboard
	{
		float3 Direction = ParentParticleSystem->CameraPosition - Properties.Position;
		Direction.x = Properties.Position.x;
		Properties.Rotation = Quat::LookAt(float3(0.0f, 0.0f, 1.0f), Direction, float3(0.0f, 1.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
		break;
	}
	}
}

inline void Particle::CalculateStatesInterpolation()
{
	CalculatePosition(Properties.LifetimeActual);
	CalculateGravity(Properties.LifetimeActual, Properties.LifetimeMax);
	CalculateSize(Properties.LifetimeActual, Properties.LifetimeMax);
	CalculateColor(Properties.LifetimeActual, Properties.LifetimeMax);
}

inline void Particle::CalculatePosition(float LifetimeFloat)
{
	Properties.Speed += Properties.force * LifetimeFloat;
	Properties.Position += Properties.Speed * LifetimeFloat;

	//Particle Position
	/*
	float dt2half = dt * dt * 0.5f;
	float alpha = atan(Properties.EmissionDirection.y / Properties.EmissionDirection.x);
	float beta = atan(Properties.EmissionDirection.x / Properties.EmissionDirection.z);
	float gamma = atan(Properties.ExternalForce.y / Properties.ExternalForce.x);
	float delta = atan(Properties.ExternalForce.x / Properties.ExternalForce.z);
	Properties.Position.x = Properties.OriginalPosition.x + Properties.Speed * cos(alpha) * cos(beta) * dt + Properties.ExternalForce.Length() * cos(gamma) * cos(delta) * dt2half;
	Properties.Position.y = Properties.OriginalPosition.y + Properties.Speed * sin(alpha) * dt + Properties.ExternalForce.Length() * sin(gamma) * dt2half;
	Properties.Position.z = Properties.OriginalPosition.z + Properties.Speed * cos(alpha) * sin(beta) * dt + Properties.ExternalForce.Length() * cos(gamma) * sin(delta) * dt2half;
	*/
}

inline void Particle::CalculateGravity(float LifetimeFloat, float MaxLifetimeFloat)
{
	Properties.force = ParticleSystem_Lerp(InitialState.force, FinalState.force, LifetimeFloat, MaxLifetimeFloat);
}

inline void Particle::CalculateSize(float LifetimeFloat, float MaxLifetimeFloat)
{
	Properties.Size = ParticleSystem_Lerp(InitialState.Size, FinalState.Size, LifetimeFloat, MaxLifetimeFloat);
}

inline void Particle::CalculateColor(float LifetimeFloat, float MaxLifetimeFloat)
{
	Properties.RGBATint = ParticleSystem_Lerp(InitialState.RGBATint, FinalState.RGBATint, LifetimeFloat, MaxLifetimeFloat);
}

ParticleMeshData::ParticleMeshData()
{

}

ParticleMeshData::~ParticleMeshData()
{
	Clean();
}

void ParticleMeshData::Copy(ParticleMeshData& Other)
{
	Clean();
	num_faces = Other.num_faces;
	id_vertices = Other.id_vertices;
	num_vertices = Other.num_vertices;
	vertices = new float[num_vertices * 3];
	memcpy(vertices, Other.vertices, sizeof(float) * num_vertices * 3);
	id_indices = Other.id_indices;
	num_indices = Other.num_indices;
	indices = new unsigned int[num_indices];
	memcpy(indices, Other.indices, sizeof(float) * num_indices);
	id_normals = Other.id_normals;
	normals = new float[num_vertices * 3];
	memcpy(normals, Other.normals, sizeof(float) * num_vertices * 3);
	/*
	id_texture_coords = Other.id_texture_coords;
	texture_coords = new float[num_vertices * 3];
	memcpy(texture_coords, Other.texture_coords, sizeof(float) * num_vertices * 3);
	*/
}

void ParticleMeshData::Clean()
{
	num_faces = 0;
	if (vertices != nullptr)
	{
		if (id_vertices > 0)
			glDeleteBuffers(1, &id_vertices);
		id_vertices = 0;
		RELEASE_ARRAY(vertices);
	}
	num_indices = 0;
	if (indices != nullptr)
	{
		if (id_indices > 0)
			glDeleteBuffers(1, &id_indices);
		id_indices = 0;
		RELEASE_ARRAY(indices);
	}
	if (normals != nullptr)
	{
		if (id_normals > 0)
			glDeleteBuffers(1, &id_normals);
		id_normals = 0;
		RELEASE_ARRAY(normals);
	}
	/*
	if (texture_coords != nullptr)
	{
		if (id_texture_coords > 0)
			glDeleteBuffers(1, &id_texture_coords);
		id_texture_coords = 0;
		RELEASE_ARRAY(texture_coords);
	}
	*/
}

void ParticleTextureData::Set(unsigned int ID, unsigned int width, unsigned int heigth, int columns, int rows, int numberOfFrames, unsigned int AnimationOrder)
{
	TextureID = ID;
	TextureW = width;
	TextureH = heigth;
	this->columns = columns;
	this->rows = rows;
	this->numberOfFrames = numberOfFrames;
	this->AnimationOrder = (OrderAnimation)AnimationOrder;
}

ParticleSystem::ParticleSystem()
{
	SetMeshResourcePlane();
}

ParticleSystem::~ParticleSystem()
{

}

bool ParticleSystem::PreUpdate(float dt)
{
	if (GenerateBuffers)
	{
		GenerateMeshResourceBuffers();
		GenerateTexturesUVs();
	}
	
	bool ret = true;
	for (std::list<Particle*>::iterator item = Particles.begin(); item != Particles.cend() && ret == true;)
	{
		bool to_delete = false;
	
		//Check if particle is alive
	  /*if ((*item)->isDead())
		{
			Emitter.ParticleNumber--;
			RELEASE(*item);
			item = Particles.erase(item);
			continue;
		}*/
		
		(*item)->CameraDistance = (long double)((CameraPosition - (*item)->Properties.Position).Length());
		ret = (*item)->PreUpdate(dt);
		++item;		
	}

	return ret;
}

bool ParticleSystem::Update(float dt)
{
	if (ShowEmitter) DebugDrawEmitter();
	if (ShowEmitterBoundBox) DebugDrawEmitterAABB();

	Emitter.EmissionDuration += dt;

	/*
	float OneParticleEach = 1.0f / (float)Emitter.SpawnRate;
	if (Emitter.EmissionDuration >= (Emitter.ParticleNumber * OneParticleEach))
	{
		CreateParticle();
		Emitter.ParticleNumber++;
	}
	*/

	float SpawnRate = 1.0f / (float)Emitter.SpawnRate;

	if ((Emitter.EmitterLifeMax <= 0.0f) || Emitter.Loop)
	{
		if ((Emitter.SpawnRate > 0) && (NextParticleTime < Emitter.EmissionDuration))
		{
			if (SpawnRate < dt)
			{
				unsigned int ParticlesToSpawn = dt / SpawnRate;
				for (unsigned int i = 0; i < ParticlesToSpawn; i++)
					CreateParticle();
			}
			else
				CreateParticle();
			NextParticleTime = Emitter.EmissionDuration + SpawnRate;
		}
	}
	else
	{
		if (Emitter.EmitterLifeMax > Emitter.EmitterLife)
		{
			if ((Emitter.SpawnRate > 0) && (NextParticleTime < Emitter.EmissionDuration))
			{
				if (SpawnRate < dt)
				{
					unsigned int ParticlesToSpawn = dt / SpawnRate;
					for (unsigned int i = 0; i < ParticlesToSpawn; i++)
						CreateParticle();
				}
				else
					CreateParticle();
				NextParticleTime = Emitter.EmissionDuration + SpawnRate;

				//CreateParticle();
				//NextParticleTime = Emitter.EmissionDuration + (1.0f / (float)Emitter.SpawnRate);
			}
			Emitter.EmitterLife += dt;
		}
	}

	//First time using Lambda operator, so i left a short explanation to don't forget what exactly is.
	//[]->Lambda -> Shortcut to a function (or object that ca be called as a function, defined by operator ())
	//Enables to bypass the creation of a function object, usefull to encapsulate short codes passed to algorithms.
	//So we use lambda operator here to send to the algorithm sort of std::list, the max and min values to search,
	//and the function used to compare values in the algorithm.
	//This sort can be modified using > operator override, and changing the sort function used
	Particles.sort([](const Particle* a, const Particle* b) { return a->CameraDistance > b->CameraDistance; });

	bool ret = true;
	for (std::list<Particle*>::iterator item = Particles.begin(); item != Particles.cend() && ret == true; ++item)
		ret = (*item)->Update(dt);
	return ret;
}

bool ParticleSystem::PostUpdate(float dt)
{
	bool ret = true;
	for (std::list<Particle*>::iterator item = Particles.begin(); item != Particles.cend() && ret == true; item++)
	{
		ret = (*item)->PostUpdate(dt);
	}
	return ret;
}

bool ParticleSystem::CleanUp()
{
	for (std::list<Particle*>::iterator item = Particles.begin(); item != Particles.cend(); ++item)
		RELEASE(*item);
	Particles.clear();
	return true;
}

/*
void ParticleSystem::SetMeshResource(ParticleMeshData& MeshData)
{
	ParticleMesh.Copy(MeshData);
	//ToDeleteBool_FirstTimeTest = true;
	//for (std::vector<Particle*>::iterator item = Particles.begin(); item != Particles.cend(); ++item) (*item)->MeshChanged = true;
}
*/

void ParticleSystem::SetMeshResourcePlane()
{
	ParticleMesh.Clean();
	ParticleMesh.num_faces = 2;
	// Vertices
	ParticleMesh.num_vertices = 4;
	ParticleMesh.vertices = new float[ParticleMesh.num_vertices * 3];
	float vertices[] =
	{
		-0.5f,  0.5f, 0,
		0.5f,  0.5f, 0,
		-0.5f, -0.5f, 0,
		0.5f, -0.5f, 0
	};
	memcpy(ParticleMesh.vertices, vertices, sizeof(float) * ParticleMesh.num_vertices * 3);
	//Indices
	ParticleMesh.num_indices = 6;
	ParticleMesh.indices = new unsigned int[ParticleMesh.num_indices];
	unsigned int indices[] =
	{
		2, 1, 0,
		2, 3, 1
	};
	memcpy(ParticleMesh.indices, indices, sizeof(float) * ParticleMesh.num_indices);
	//Normals
	ParticleMesh.normals = new float[ParticleMesh.num_vertices * 3];
	float normals[] =
	{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};
	memcpy(ParticleMesh.normals, normals, sizeof(float) * ParticleMesh.num_vertices * 3);
	// Texture coords
	/*
	ParticleMesh.texture_coords = new float[ParticleMesh.num_vertices * 3];
	float texture_coords[] =
	{
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};
	memcpy(ParticleMesh.texture_coords, texture_coords, sizeof(float) * ParticleMesh.num_vertices * 3);
	*/
	GenerateBuffers = true;
	for (std::list<Particle*>::iterator item = Particles.begin(); item != Particles.cend(); ++item) (*item)->MeshChanged = true;
}

const ParticleTextureData * ParticleSystem::GetTextureResource() const
{
	return &TextureData;
}

void ParticleSystem::SetTextureResource(unsigned int ID, unsigned int width, unsigned int heigth, int columns, int rows, int numberOfFrames, unsigned int AnimationOrder)
{
	TextureData.Set(ID, width, heigth, columns, rows, numberOfFrames, AnimationOrder);
	GenerateTexturesUVs();
	Emitter.EmitterLife = 0.0f;
	NextParticleTime = 0.0f;
	//Emitter.EmissionDuration = 0.0f;
	CleanUp(); //Delete all active particles
}

void ParticleSystem::SetInitialStateResource(const ParticleState& state)
{
	InitialState = state;
	Emitter.EmitterLife = 0.0f;
	NextParticleTime = 0.0f;
	//Emitter.EmissionDuration = 0.0f;
	CleanUp(); //Delete all active particles
}

void ParticleSystem::SetFinalStateResource(const ParticleState& state)
{
	FinalState = state;
	Emitter.EmitterLife = 0.0f;
	NextParticleTime = 0.0f;
	//Emitter.EmissionDuration = 0.0f;
	CleanUp(); //Delete all active particles
}

void ParticleSystem::SetEmitterResource(const ParticleEmitter& emitter)
{
	Emitter = emitter;
	Emitter.EmitterLife = 0.0f;
	NextParticleTime = 0.0f;
	//Emitter.EmissionDuration = 0.0f;
	CleanUp(); //Delete all active particles
}

void ParticleSystem::GetInitialState(ParticleState & state) const
{
	state = InitialState;
}

void ParticleSystem::GetFinalState(ParticleState & state) const
{
	state = FinalState;
}

void ParticleSystem::GetEmitter(ParticleEmitter & emitter) const
{
	emitter = Emitter;
}

ParticleState * ParticleSystem::GetInitialState()
{
	return &InitialState;
}

ParticleState * ParticleSystem::GetFinalState()
{
	return &FinalState;
}

const ParticleEmitter * ParticleSystem::GetEmitter() const
{
	return &Emitter;
}

void ParticleSystem::SetEmitterTransform(const float4x4 & transform)
{
	Emitter.SetTransform(transform);
}

void ParticleSystem::DebugDrawEmitter()
{
	Emitter.DebugDrawEmitter();
}

void ParticleSystem::DebugDrawEmitterAABB()
{
	Emitter.DebugDrawEmitterAABB();
}

void ParticleSystem::DrawImGuiEditorWindow()
{
	if (EditorWindowOpen)
	{
		ImGui::Begin("Particle Editor", &EditorWindowOpen, ImVec2(1200, 800));

		ImVec2 WindowSize = ImGui::GetWindowSize();
		float ChildsWidth = (WindowSize.x - 28.0f) * 0.33f;
		float ChildsHeight = (WindowSize.y - 52.0f) * 0.5f;

		ImGui::BeginChild("Preview", ImVec2(ChildsWidth, ChildsHeight), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		DrawTexturePreview();
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Initial State", ImVec2(ChildsWidth, ChildsHeight), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		ParticleStateEnum = InitialState_Enum;
		DrawColorSelector();
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Final State", ImVec2(ChildsWidth, ChildsHeight), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		ParticleStateEnum = FinalState_Enum;
		DrawColorSelector();
		ImGui::EndChild();

		ImGui::BeginChild("EmitterGeneralOptions", ImVec2(0, ChildsHeight), true);
		DrawEmitterOptions();
		ImGui::EndChild();

		ImGui::End();
	}
}

AABB& ParticleSystem::GetEmitterAABB()
{
	return Emitter.BoundingBox;
}

const ParticleMeshData & ParticleSystem::GetParticleMeshData() const
{
	return ParticleMesh;
}

void ParticleSystem::SetCameraPosToFollow(float3 position)
{
	CameraPosition = position;
}

unsigned int ParticleSystem::GetTextureID(float MaxParticleLife, float time)
{
	unsigned int ID = (unsigned int)(time / (MaxParticleLife / (float)TextureData.numberOfFrames));
	return TexturesUV_ID[CLAMP(ID, 0, TexturesUV_ID.size() - 1)];
}

bool ParticleSystem::IsEmitterDead() const
{
	bool ret = false;
	if ((Emitter.EmitterLifeMax >= 0.0f) && !Emitter.Loop)
		if (Emitter.EmitterLife > Emitter.EmitterLifeMax)
			ret = true;
	return ret;
}

void ParticleSystem::GenerateMeshResourceBuffers()
{
	// Vertices Buffer
	glGenBuffers(1, (GLuint*)&ParticleMesh.id_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, ParticleMesh.id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ParticleMesh.num_vertices * 3, ParticleMesh.vertices, GL_STATIC_DRAW);
	// Indices Buffer
	glGenBuffers(1, (GLuint*)&ParticleMesh.id_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ParticleMesh.id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ParticleMesh.num_indices, ParticleMesh.indices, GL_STATIC_DRAW);
	// Indices Buffer
	glGenBuffers(1, (GLuint*)&ParticleMesh.id_normals);
	glBindBuffer(GL_ARRAY_BUFFER, ParticleMesh.id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ParticleMesh.num_vertices * 3, ParticleMesh.normals, GL_STATIC_DRAW);
	// Texture coords Buffer
	/*
	glGenBuffers(1, (GLuint*)&ParticleMesh.id_texture_coords);
	glBindBuffer(GL_ARRAY_BUFFER, ParticleMesh.id_texture_coords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ParticleMesh.num_indices * 3, ParticleMesh.texture_coords, GL_STATIC_DRAW);
	*/
	for (std::list<Particle*>::iterator item = Particles.begin(); item != Particles.cend(); ++item) (*item)->MeshChanged = false;
	GenerateBuffers = false;
}

void ParticleSystem::GenerateUVBuffers()
{
	for (std::vector<unsigned int>::iterator item = TexturesUV_ID.begin(); item != TexturesUV_ID.cend(); ++item)
		if (*item > 0) glDeleteBuffers(1, &(*item));
	TexturesUV_ID.clear();
	//for (std::vector<float*>::iterator item = TexturesUV_Data_ptr.begin(); item != TexturesUV_Data_ptr.cend(); ++item)
	//	RELEASE_ARRAY(*item);
	//TexturesUV_Data_ptr.clear();

	for (unsigned int i = 0; i < TextureData.columns * TextureData.rows; i++)
	{
		unsigned int NewID = 0;

		//float* texture_coords_ptr = new float[ParticleMesh.num_vertices * 3];
		float texture_coords[] =
		{
			TexturesUV_Data[i].z, TexturesUV_Data[i].y, 0.0f,
			TexturesUV_Data[i].z, TexturesUV_Data[i].w, 0.0f,
			TexturesUV_Data[i].x, TexturesUV_Data[i].y, 0.0f,
			TexturesUV_Data[i].x, TexturesUV_Data[i].w, 0.0f
		};
		//memcpy(texture_coords_ptr, texture_coords, sizeof(float) * ParticleMesh.num_vertices * 3);
		//TexturesUV_Data_ptr.push_back(texture_coords_ptr);

		glGenBuffers(1, (GLuint*)&NewID);
		glBindBuffer(GL_ARRAY_BUFFER, NewID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * ParticleMesh.num_indices * 3, texture_coords, GL_STATIC_DRAW);

		TexturesUV_ID.push_back(NewID);
	}
}

void ParticleSystem::GenerateTexturesUVs()
{
	TexturesUV_Data.clear();
	for (unsigned int i = 0; i < TextureData.columns * TextureData.rows; i++)
	{
		float4 NewUV = float4::zero;
		float2 ColumRow = float2::zero;
		switch (TextureData.AnimationOrder)
		{
		case 0: //Right
			ColumRow.x = i / TextureData.columns; //Row
			ColumRow.y = i % TextureData.columns; //Column
			break;
		case 1: //Down
			ColumRow.x = i % TextureData.columns; //Column
			ColumRow.y = i / TextureData.columns; //Row
			break;
		}
		NewUV.x = (1.0f / (float)TextureData.columns) * ColumRow.y;
		NewUV.y = (1.0f / (float)TextureData.rows) * ((float)TextureData.rows - ColumRow.x - 1.0f);
		NewUV.z = (1.0f / (float)TextureData.columns) * (ColumRow.y + 1.0f);
		NewUV.w = (1.0f / (float)TextureData.rows) * ((float)TextureData.rows - ColumRow.x);
		TexturesUV_Data.push_back(NewUV);
	}
	GenerateUVBuffers();
}

void ParticleSystem::DrawTexturePreview()
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	static ImVec2 InitialPos = ImVec2(0.0f, 0.0f);
	static ImVec2 FinalPos = ImVec2(0.0f, 0.0f);

	ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
	ImVec2 region_avaliable = ImGui::GetContentRegionAvail();
	float TexW = 250.0f;
	float TexH = 250.0f;
	float scale = 1.0f;
	static float focus_sz = 50.0f;

	if (TextureData.TextureW > 0.0f && TextureData.TextureH > 0.0f)
	{
		scale = region_avaliable.x / TextureData.TextureW;
		TexW = region_avaliable.x;
		TexH = TextureData.TextureH * scale;
	}

	ImVec2 canvas_size = ImVec2(TexW, TexH);        // Resize canvas to what's available
	float tex_w = canvas_pos.x + TextureData.TextureW;		//	if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
	float tex_h = canvas_pos.y + TextureData.TextureH;		//	if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;
	ImTextureID tex_id = (void*)TextureData.TextureID;

	draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), ImColor(50, 50, 50), ImColor(50, 50, 60), ImColor(60, 60, 70), ImColor(50, 50, 60));
	bool adding_preview = false;
	ImGui::InvisibleButton("canvas", canvas_size);
	ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	draw_list->PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y));      // clip lines within the canvas (if we resize it, etc.)
	draw_list->AddImage((void*)TextureData.TextureID, canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), ImVec2(0, 1), ImVec2(1, 0)); // * TextureData.TextureW & H * texSize if uncommented
	draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), ImColor(255, 255, 255));
	if (mouse_pos_in_canvas.x > canvas_pos.x && mouse_pos_in_canvas.y > canvas_pos.y && mouse_pos_in_canvas.x < canvas_pos.x + canvas_size.x && mouse_pos_in_canvas.y < canvas_pos.y + canvas_size.y)
	{
		/*
		if (MouseLeftClick.State == KeyInput::Down)
		InitialPos = ImVec2(mouse_pos_in_canvas.x - canvas_pos.x, mouse_pos_in_canvas.y - canvas_pos.y);
		if (MouseLeftClick.State == KeyInput::Repeat)
		FinalPos = ImVec2(mouse_pos_in_canvas.x - canvas_pos.x, mouse_pos_in_canvas.y - canvas_pos.y);
		*/

		ImGui::BeginTooltip();
		float focus_x = mouse_pos_in_canvas.x - canvas_pos.x - focus_sz * 0.5f; if (focus_x < 0.0f) focus_x = 0.0f; else if (focus_x > canvas_size.x - focus_sz) focus_x = canvas_size.x - focus_sz;
		float focus_y = mouse_pos_in_canvas.y - canvas_pos.y - focus_sz * 0.5f; if (focus_y < 0.0f) focus_y = 0.0f; else if (focus_y > canvas_size.y - focus_sz) focus_y = canvas_size.y - focus_sz;
		ImVec2 uv0 = ImVec2((focus_x) / canvas_size.x, (focus_y) / canvas_size.y);
		ImVec2 uv1 = ImVec2((focus_x + focus_sz) / canvas_size.x, (focus_y + focus_sz) / canvas_size.y);
		uv0 = ImVec2(uv0.x, 1.0f - uv0.y);
		uv1 = ImVec2(uv1.x, 1.0f - uv1.y);
		ImGui::Image(tex_id, ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		ImGui::EndTooltip();
	}
	//draw_list->AddRect(ImVec2(InitialPos.x + canvas_pos.x, InitialPos.y + canvas_pos.y), ImVec2(FinalPos.x + canvas_pos.x, FinalPos.y + canvas_pos.y), IM_COL32(255, 255, 0, 255), 0.0f, ~0);
	draw_list->PopClipRect();
	for (int i = 1; i < TextureData.columns; i++)
		draw_list->AddLine(ImVec2(canvas_pos.x + (canvas_size.x / TextureData.columns) * i, canvas_pos.y), ImVec2(canvas_pos.x + (canvas_size.x / TextureData.columns) * i, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 0, 255));
	for (int i = 1; i < TextureData.rows; i++)
		draw_list->AddLine(ImVec2(canvas_pos.x, canvas_pos.y + (canvas_size.y / TextureData.rows) * i), ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + (canvas_size.y / TextureData.rows) * i), IM_COL32(255, 255, 0, 255));
	ImGui::PushItemWidth(95);
	if (ImGui::SliderInt("Columns", &TextureData.columns, 1, 20))
	{
		TextureData.numberOfFrames = TextureData.rows * TextureData.columns;
		GenerateTexturesUVs();
	}
	ImGui::SameLine();
	if (ImGui::SliderInt("Rows", &TextureData.rows, 1, 20))
	{
		TextureData.numberOfFrames = TextureData.rows * TextureData.columns;
		GenerateTexturesUVs();
	}
	ImGui::PopItemWidth();
	float maxFrames = TextureData.rows * TextureData.columns;
	if (maxFrames > 1)
	{
		ImGui::PushItemWidth(140);
		if (ImGui::DragInt("Number of Frames", &TextureData.numberOfFrames, 0.1f, 1, maxFrames)) GenerateTexturesUVs();
		ImGui::PopItemWidth();
	}
	ImGui::PushItemWidth(80);
	if (ImGui::Combo("Animation Order", (int*)&TextureData.AnimationOrder, "Right\0Down\0")) GenerateTexturesUVs();
	ImGui::PopItemWidth();
	//ImGui::DragFloat("Zoom", &focus_sz, 0.001f, 1.0f, 100.0f);
}

void ParticleSystem::DrawColorSelector()
{
	ParticleState* state = &InitialState;
	if (ParticleStateEnum == InitialState_Enum)
		ImGui::Text("Initial State");
	else
	{
		ImGui::Text("Final State");
		state = &FinalState;
	}
	ImGui::Separator();
	int misc_flags = ((state->alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (state->alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (state->options_menu ? 0 : ImGuiColorEditFlags_NoOptions));
	ImGui::Combo("Inputs Mode", &state->inputs_mode, "All Inputs\0No Inputs\0RGB Input\0HSV Input\0HEX Input\0");
	ImGui::Combo("Picker Mode", &state->picker_mode, "Auto\0Square selector\0Triangle wheel\0");
	ImGuiColorEditFlags flags = misc_flags;
	if (!state->alpha) flags |= ImGuiColorEditFlags_NoAlpha; // This is by default if you call ColorPicker3() instead of ColorPicker4()
	if (state->alpha_bar) flags |= ImGuiColorEditFlags_AlphaBar;
	if (state->side_preview) flags |= ImGuiColorEditFlags_NoSidePreview;
	if (state->picker_mode == 1) flags |= ImGuiColorEditFlags_PickerHueBar;
	if (state->picker_mode == 2) flags |= ImGuiColorEditFlags_PickerHueWheel;
	if (state->inputs_mode == 1) flags |= ImGuiColorEditFlags_NoInputs;
	if (state->inputs_mode == 2) flags |= ImGuiColorEditFlags_RGB;
	if (state->inputs_mode == 3) flags |= ImGuiColorEditFlags_HSV;
	if (state->inputs_mode == 4) flags |= ImGuiColorEditFlags_HEX;
	ImGui::ColorPicker4("Color##Color", (float*)&state->RGBATint, flags);
	ImGui::PushItemWidth(80);
	ImGui::DragFloat("+-##Size", &state->Size, 0.001f, 0.0f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("Size+-Var##SizeVariation", &state->SizeVariation, 0.001f, 0.0f, 100.0f);
	ImGui::PopItemWidth();
	ImGui::DragFloat4("Color Var##ColorVariation", (float*)&state->RGBATintVariation, 0.01f, 0.0f, 1.0f);
	ImGui::PushItemWidth(175);
	ImGui::DragFloat3("Gravity##Gravity", &state->force[0], 0.01, -10.0f, 10.0f, "%.2f");
	ImGui::DragFloat3("Gravity Var##GravityVariation", &state->forceVariation[0], 0.01, -10.0f, 10.0f, "%.2f");
	ImGui::PopItemWidth();
}

void ParticleSystem::DrawEmitterOptions()
{
	ImGui::Columns(3, "##mycolumns3", false);  // 2-ways, no border
	ImGui::PushItemWidth(100);
	if (ImGui::Combo("Emitter Shape", (int*)&Emitter.Type, "Sphere\0SemiSphere\0Cone\0Box\0Circle")) Emitter.ResetEmitterValues();
	ImGui::PopItemWidth();
	switch (Emitter.Type)
	{
	case 0: //EmitterType_Sphere
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("Radius", (float*)&Emitter.EmitterShape.Sphere_Shape.r, 0.1f, 0.0f, 100.0f);
		//ImGui::PopItemWidth();
		//ImGui::PushItemWidth(200);
		//ImGui::DragFloat3("Center Point", (float*)&Emitter.EmitterShape.Sphere_Shape.pos, 0.1f, 0.0f, 100.0f);
		ImGui::PopItemWidth();
		break;
	case 1: //EmitterType_SemiSphere
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("Radius", (float*)&Emitter.EmitterShape.Sphere_Shape.r, 0.1f, 0.0f, 100.0f);
		//ImGui::PopItemWidth();
		//ImGui::PushItemWidth(200);
		//ImGui::DragFloat3("Center Point", (float*)&Emitter.EmitterShape.Sphere_Shape.pos, 0.1f, 0.0f, 100.0f);
		ImGui::PopItemWidth();
		break;
	case 2: //EmitterType_Cone
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("Aperture Radius", (float*)&Emitter.EmitterShape.ConeTrunk_Shape.Upper_Circle.r, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Base Radius", (float*)&Emitter.EmitterShape.ConeTrunk_Shape.Bottom_Circle.r, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Cone Heigth", (float*)&Emitter.EmitterShape.ConeTrunk_Shape.heigth, 0.1f, 0.0f, 100.0f);
		ImGui::PopItemWidth();
		break;
	case 3: //EmitterType_Box
	{
		ImGui::PushItemWidth(200);
		static float SizeX = 1.0f;
		if (ImGui::DragFloat("Size X", &SizeX, 0.1f, 0.0f, 100.0f))
		{
			Emitter.EmitterShape.Box_Shape.maxPoint.x = SizeX / 2.0f;
			Emitter.EmitterShape.Box_Shape.minPoint.x = -SizeX / 2.0f;
		}
		static float SizeY = 1.0f;
		if (ImGui::DragFloat("Size Y", &SizeY, 0.1f, 0.0f, 100.0f))
		{
			Emitter.EmitterShape.Box_Shape.maxPoint.y = SizeY / 2.0f;
			Emitter.EmitterShape.Box_Shape.minPoint.y = -SizeY / 2.0f;
		}
		static float SizeZ = 1.0f;
		if (ImGui::DragFloat("Size Z", &SizeZ, 0.1f, 0.0f, 100.0f))
		{
			Emitter.EmitterShape.Box_Shape.maxPoint.z = SizeZ / 2.0f;
			Emitter.EmitterShape.Box_Shape.minPoint.z = -SizeZ / 2.0f;
		}
		ImGui::PopItemWidth();
		break;
	}
	case 4: //EmitterType_Circle
		ImGui::PushItemWidth(100);
		ImGui::DragFloat("Radius", (float*)&Emitter.EmitterShape.Circle_Shape.r, 0.1f, 0.0f, 100.0f);
		//ImGui::PopItemWidth();
		//ImGui::PushItemWidth(200);
		//ImGui::DragFloat3("Center Point", (float*)&Emitter.EmitterShape.Circle_Shape.pos, 0.1f, 0.0f, 100.0f);
		ImGui::PopItemWidth();
		break;
	}
	ImGui::NextColumn();
	ImGui::PushItemWidth(80);
	ImGui::DragFloat("Emitter Life", &Emitter.EmitterLifeMax, 0.1f, -1.0f, 120.0f);
	ImGui::SliderInt("Particles emitted per second", (int*)&Emitter.SpawnRate, 0, 1000);
	ImGui::DragFloat("+-##Lifetime", &Emitter.Lifetime, 0.01f, 0.0f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("Lifetime +- Var##LifetimeVariation", &Emitter.LifetimeVariation, 0.01f, 0.0f, 100.0f);
	char title[100] = "";
	sprintf_s(title, 100, "Emission Duration: %.3f", Emitter.EmissionDuration);
	ImGui::Text(title);
	ImGui::Checkbox("Loop", &Emitter.Loop);
	sprintf_s(title, 100, "Particle Num: %i", Particles.size());
	ImGui::Text(title);
	ImGui::DragFloat("+-##Speed", &Emitter.Speed, 0.01f, 0.0f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("Speed +- Var##SpeedVariation", &Emitter.SpeedVariation, 0.01f, 0.0f, 100.0f);
	ImGui::PopItemWidth();
	ImGui::NextColumn();
	ImGui::PushItemWidth(175);
	//ImGui::Combo("Emyssion Type", (int*)&Emitter.EmissionType, "Local\0World\0");
	ImGui::Combo("Particle Facing Options", (int*)&Emitter.ParticleFacingOptions, "Null\0Billboard\0Vertical Billboard\0Horizontal Billboard");
	ImGui::PopItemWidth();
	//TODO: Curve editor to interpolate initial-final
	ImGui::PushItemWidth(200);
	ImGui::Checkbox("Show Emitter", &ShowEmitter);
	ImGui::Checkbox("Show Emitter Bounding Box", &ShowEmitterBoundBox);
	ImGui::DragFloat3("AABB Max", &Emitter.BoundingBox.maxPoint[0], 0.01f, -100.0f, 100.0f);
	ImGui::DragFloat3("AABB Min", &Emitter.BoundingBox.minPoint[0], 0.01f, -100.0f, 100.0f);
	ImGui::PopItemWidth();
}

bool ParticleSystem::CreateParticle()
{
	LCG RandGen;
	float3 Direction = float3::zero;
	float3 offset = float3::zero;
	switch (Emitter.Type)
	{
	case 0: //EmitterType_Sphere
		Direction = Emitter.EmitterShape.Sphere_Shape.RandomPointOnSurface(RandGen);
		break;
	case 1: //EmitterType_SemiSphere
		Direction = Emitter.EmitterShape.Sphere_Shape.RandomPointOnSurface(RandGen);
		Direction.y = abs(Direction.y);
		break;
	case 2: //EmitterType_Cone
	{
		float3 BasePoint = float3::zero;
		float3 TopPoint = float3(0.0f, Emitter.EmitterShape.ConeTrunk_Shape.heigth, 0.0f);
		if (Emitter.EmitterShape.ConeTrunk_Shape.Bottom_Circle.r > 0.0f)
		{
			float radius = RandGen.Float(0.0f, Emitter.EmitterShape.ConeTrunk_Shape.Bottom_Circle.r);
			float angle = RandGen.Float(0.0f, 360.0f) * DEGTORAD;
			BasePoint = float3(cos(angle) * radius, 0.0f, sin(angle) * radius);
		}
		if (Emitter.EmitterShape.ConeTrunk_Shape.Upper_Circle.r > 0.0f)
			TopPoint = (Emitter.EmitterShape.ConeTrunk_Shape.Upper_Circle.r * BasePoint) / Emitter.EmitterShape.ConeTrunk_Shape.Bottom_Circle.r;
		TopPoint.y = Emitter.EmitterShape.ConeTrunk_Shape.heigth;
		offset = BasePoint;
		Direction = TopPoint - BasePoint;
		break;
	}
	case 3: //EmitterType_Box
	{
		float SizeX = abs(Emitter.EmitterShape.Box_Shape.maxPoint.x - Emitter.EmitterShape.Box_Shape.minPoint.x);
		float SizeY = abs(Emitter.EmitterShape.Box_Shape.maxPoint.y - Emitter.EmitterShape.Box_Shape.minPoint.y);
		float SizeZ = abs(Emitter.EmitterShape.Box_Shape.maxPoint.z - Emitter.EmitterShape.Box_Shape.minPoint.z);
		offset.x = RandGen.Float(SizeX / 2.0f, -SizeX / 2.0f);
		offset.z = RandGen.Float(SizeZ / 2.0f, -SizeZ / 2.0f);
		Direction = float3(0.0f, SizeY, 0.0f);
		break;
	}
	case 4: //EmitterType_Circle
		float angleRadians = RandGen.Float(0.0f, 360.0f * DEGTORAD);
		Direction = Emitter.EmitterShape.Circle_Shape.GetPoint(angleRadians);
		break;
	}

	Quat Rotation = Quat::identity;
	Emitter.GetRotation(Rotation);
	Direction.Normalize();
	Quat Rot = Rotation * Quat(Direction, 1.0f) *Rotation.Conjugated();
	Rot.Normalize();
	Direction = float3(Rot.x, Rot.y, Rot.z);

	Particle* NewParticle = new Particle(this, InitialState, FinalState, Direction * (Emitter.Speed + RandGen.Float(-Emitter.SpeedVariation, Emitter.SpeedVariation)), offset, Emitter.Lifetime + RandGen.Float(-Emitter.LifetimeVariation, Emitter.LifetimeVariation));
	//Particle* NewParticle = new Particle(this, InitialState, FinalState, Direction.Normalized() * (Emitter.Speed + RandGen.Float(-Emitter.SpeedVariation, Emitter.SpeedVariation)), Emitter.Lifetime + RandGen.Float(-Emitter.LifetimeVariation, Emitter.LifetimeVariation));
	Particles.push_back(NewParticle);
	return true;
}