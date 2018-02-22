#ifndef __ParticleSystem_H__
#define __ParticleSystem_H__

#include <vector>
#include <list>
#include "MathGeoLib.h"


#ifdef NULL
#undef NULL
#endif
#define NULL  0

#ifdef DEGTORAD
#undef DEGTORAD
#endif
#define DEGTORAD 0.0174532925199432957f

#ifdef RADTODEG
#undef RADTODEG
#endif
#define RADTODEG 57.295779513082320876f

// Deletes a buffer
#ifdef RELEASE
#undef RELEASE
#endif
#define RELEASE( x )	\
{                       \
	if( x != NULL )     \
	{                   \
		delete x;       \
	    x = NULL;       \
	}                   \
}

// Deletes an array of buffers
#ifdef RELEASE_ARRAY
#undef RELEASE_ARRAY
#endif
#define RELEASE_ARRAY( x )	\
{                           \
	if( x != NULL )         \
	{                       \
		delete[] x;         \
	    x = NULL;           \
	}                       \
}

#ifdef CLAMP
#undef CLAMP
#endif
#define CLAMP(n,min,max) ((n <= min) ? n=min : (n >= max) ? n=max : n=n)

//Debug draw defines
#define DEBUG_THICKNESS 4.0f
#define DEBUG_COLOR_R 1.0f
#define DEBUG_COLOR_G 1.0f
#define DEBUG_COLOR_B 0.0f
#define CIRCLES_SEGMENTS 10.0f

struct ConeTrunk			//Definition of a cone trunk for the cone emitter type
{
	Circle Upper_Circle;
	Circle Bottom_Circle;
	float heigth = 1.0f;
};

class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	void DebugDrawEmitter();						//Draw emitter shape
	void DebugDrawEmitterAABB();					//Draw emitter AABB
	void ResetEmitterValues();						//Reset emitter values

	void SetTransform(const float4x4& transform);	//Set this emitter transform

	void GetPosition(float3& pos);					//Get this emitter position
	void GetRotation(Quat& rot);					//Get this emitter rotation
	void GetScale(float3& sca);						//Get this emitter scale

private:
	void DrawSphere(const Sphere& shape);			//Draw sphere emitter
	void DrawSemiSphere(const Sphere& shape);		//Draw semisphere emitter
	void DrawCone(const ConeTrunk& shape);			//Draw cone emitter
	void DrawBox(const AABB& shape);				//Draw box emitter
	void DrawCircle(const Circle& shape);			//Draw circle emitter

public:
	float EmitterLifeMax = -1.0f;					//EmitterLife, after that, we can execute another child particle system, if -1, infinite life
	float EmitterLife = 0.0f;						//Emitter Real Life
	float4x4 Transform = float4x4::identity;		//Transformation of this emitter
	unsigned int SpawnRate = 2;						//How many particles are emitted every second
	float Lifetime = 1.0f;							//Lifetime of emitted particles
	float LifetimeVariation = 0.0f;					//Lifetime variation of emitted particles
	float EmissionDuration = 0.0f;					//If loop is false, emission is played EmissionDuration
	bool Loop = true;								//Ignore EmissionDuration and keep emitting
	int ParticleNumber = 0;							//Alive particles emitted
	float Speed = 5.0f;								//Speed of emitted particles
	float SpeedVariation = 0.0f;					//Speed variation of emitted particles
	AABB BoundingBox;								//User can set AABB for camera culling purpose (we can add physics...)

	/*
	//Not working properly, transformations errors, so to avoid malfunctionality and a
	//possible crash, all emission type is commented, all emissions are world space
	enum TypeEmission
	{
		LocalEmission,
		WorldEmission
	} EmissionType = WorldEmission;
	*/
	enum TypeEmitter
	{
		EmitterType_Sphere,
		EmitterType_SemiSphere,
		EmitterType_Cone,
		EmitterType_Box,
		EmitterType_Circle
	} Type = EmitterType_Cone;						//Emitter shape type
	enum TypeBillboard
	{
		Null,
		Billboard,
		VerticalBillboard,
		HorizontalBillboard
	} ParticleFacingOptions = Billboard;			//Emitted particles billboard type
	union EmitterShapeUnion
	{
		EmitterShapeUnion();
		AABB Box_Shape;
		Sphere Sphere_Shape;
		ConeTrunk ConeTrunk_Shape;
		Circle Circle_Shape;
	} EmitterShape;									//Emitter shape
};

struct ParticleState
{
	float3 force = float3(0.0f, -9.81f, 0.0f);			//Force that effects that particle
	float3 forceVariation = float3(0.0f, 0.0f, 0.0f);	//Force that effects that particle variation
	float Size = 1.0f;									//Size of the particle
	float SizeVariation = 0.0f;							//Size of the particle variation
	float4 RGBATint = float4::one;						//Particle Texture tint
	float4 RGBATintVariation = float4::zero;			//Particle Texture tint variation

	//Variables to store values of imgui elements
	bool alpha_preview = true;
	bool alpha_half_preview = false;
	bool options_menu = true;
	bool alpha = true;
	bool alpha_bar = true;
	bool side_preview = true;
	int inputs_mode = 2;
	int picker_mode = 0;
};

struct ParticleAssignedState
{
	float3 force = float3::zero;		//Gravity that affects that particle
	float Size = 1.0f;					//Size that affects that particle
	float4 RGBATint = float4::one;		//Particle texture tint that affects that particle
};

struct ParticleMeshData;

struct ParticleProperties
{
	float3 Position = float3::zero;				//Particle Actual Position
	Quat Rotation = Quat::identity;				//Particle Actual Rotation
	float3 Scale = float3::one;					//Particle Actual Scale
	float Size = 1.0f;							//Scale Multiplicator to modify
	float3 Speed = float3::zero;				//Particle Speed
	float3 EmissionDirection = float3::zero;	//Particle Emission Direction
	float3 force = float3::zero;				//Force that effects that particle
	float LifetimeMax = 0;						//Max Particle Lifetime
	float LifetimeActual = 0;					//Actual Particle Lifetime
	unsigned int TextureID = 0;					//Texture ID used by this particle
	float4 RGBATint = float4::zero;				//Particle Texture tint
};

class ParticleSystem;

class Particle
{
public:
	Particle(ParticleSystem* parent, const ParticleState& Initial, const ParticleState& Final, float3 Speed, float3 offset, float LifetimeMax);
	~Particle();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);

	bool isDead();												//Check if this particle is dead, we can use this to trigger a child emitter of the particle

	void DrawParticle();										//Function to draw this particle

private:
	void SetAssignedStateFromVariables(ParticleAssignedState& AState, const ParticleState& State);		//Set particle Assigned state from Variable state
	void OrientateParticle();																			//Rotate all particles with selected billboard type
	inline void CalculateStatesInterpolation();															//Calculate particle inperpolation, call all the otehr Calculate...(); functions
	inline void CalculatePosition(float LifetimeFloat);													//Calculate particle position and speed inperpolation
	inline void CalculateGravity(float LifetimeFloat, float MaxLifetimeFloat);							//Calculate particle gravity/force inperpolation
	inline void CalculateSize(float LifetimeFloat, float MaxLifetimeFloat);								//Calculate particle size inperpolation
	inline void CalculateColor(float LifetimeFloat, float MaxLifetimeFloat);							//Calculate particle color inperpolation

public:
	ParticleSystem* ParentParticleSystem = nullptr;				//Store a pointer to parent particle system
	ParticleProperties Properties;								//Particle Properties
	ParticleAssignedState InitialState;							//Particle Initial State Properties with no variations, all are final values (calculated from ParticleState +- Var)
	ParticleAssignedState FinalState;							//Particle Final State Properties with no variations, all are final values (calculated from ParticleState +- Var)
	bool MeshChanged = false;									//If we cahnge the mesh, we need to stop drowing it, update buffers and then start again
	long double CameraDistance = 0.0;							//Store camera distance of this particle, used to sort them and draw with correct order

private:
	bool ToDelete = false;										//If this particle is dead, we set this bool to true and wait to the right time to delete it
};

struct ParticleMeshData							//Very similar to MeshDataResource, but we copy it here to separate as much as we can the particle code from engine code (easier to export/make a library)
{
	ParticleMeshData();
	~ParticleMeshData();
	void Copy(ParticleMeshData& Other);			//Copy data from other ParticleMeshData
	void Clean();								//Clear all stored values

	unsigned int num_faces = 0;					//Mesh faces number
	unsigned int id_vertices = 0;				//Vertices ID in VRAM
	unsigned int num_vertices = 0;				//Mesh vertices number
	float* vertices = nullptr;					//Mesh vertices
	unsigned int id_indices = 0;				//Indices ID in VRAM
	unsigned int num_indices = 0;				//Mesh indices number
	unsigned int* indices = nullptr;			//Mesh indices
	unsigned int id_normals = 0;				//Normals ID in VRAM
	float* normals = nullptr;					//Mesh normals
	//unsigned int id_texture_coords = 0;		//Texture coords ID in VRAM
	//float* texture_coords = nullptr;			//Mesh texture coords
};

struct ParticleTextureData
{
	void Set(unsigned int ID, unsigned int width, unsigned int heigth, int columns = 1, int rows = 1, int numberOfFrames = 1, unsigned int AnimationOrder = 0); //Set all variables

	unsigned int TextureID = 0;		//Texure ID to use
	unsigned int TextureW = 0;		//Texure width
	unsigned int TextureH = 0;		//Texure height

	int columns = 1;				//Number of colums of the loaded particle texture sheet
	int rows = 1;					//Number of rows of the loaded particle texture sheet
	int numberOfFrames = 1;			//Number of frames of the loaded particle texture sheet you are using, counted following the AnimationOrder option

	enum OrderAnimation
	{
		Right,
		Down
	}AnimationOrder = Right;		//You can set if the animation you are using, goes right or down
};

//Used to maintain this system as independent as possible, in ParticleSystem you have one instance of this to update mouse click. But now it's not used, we use
//this in previous UI iterations, but left it here if we need it in the future
struct KeyInput
{
	enum
	{
		Idle,
		Up,
		Down,
		Repeat
	}State = Idle;		//Store the state of the key/button
};

class ParticleSystem
{
	friend class Particle;										//We need to be friends of the class Particle to access it's private members and change them at will
	bool GenerateBuffers = true;								//We need to wait when creating buffers, so we set this bool to false and wait for the right time

public:
	ParticleSystem();
	~ParticleSystem();
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	//void SetMeshResource(ParticleMeshData& MeshData);			//Set Mesh resource to use
	void SetMeshResourcePlane();								//Delete actual mesh + load a plane
	const ParticleTextureData* GetTextureResource() const;		//Get the data of the loaded texture
	void SetTextureResource(unsigned int ID, unsigned int width, unsigned int heigth, int columns = 1, int rows = 1, int numberOfFrames = 1, unsigned int AnimationOrder = 0); //Set new texture with options

	void SetInitialStateResource(const ParticleState& state);	//Set Particle Initial State if you want to load it as a resource
	void SetFinalStateResource(const ParticleState& state);		//Set Particle Final State if you want to load it as a resource
	void SetEmitterResource(const ParticleEmitter& emitter);	//Set Particle Emitter if you want to load it as a resource
	void GetInitialState(ParticleState& state) const;			//Get Particle Initial State if you want to save it as a resource
	void GetFinalState(ParticleState& state) const;				//Get Particle Final State if you want to save it as a resource
	void GetEmitter(ParticleEmitter& emitter) const;			//Get Particle Emitter if you want to save it as a resource
	ParticleState* GetInitialState() ;				//Get Particle Initial State if you want to save it as a resource
	ParticleState* GetFinalState() ;					//Get Particle Final State if you want to save it as a resource
	const ParticleEmitter* GetEmitter() const;					//Get Particle Emitter if you want to save it as a resource

	void SetEmitterTransform(const float4x4& transform);		//Set emitter transformation

	void DebugDrawEmitter();									//Draw emitter shape
	void DebugDrawEmitterAABB();								//Draw emitter AABB
	void DrawImGuiEditorWindow();								//Draw Particle Editor Window

	AABB& GetEmitterAABB();										//You can get the Emitter AABB and edit min and max point with gizmos
	const ParticleMeshData& GetParticleMeshData() const;		//You can get the particle mesh data used by particles of this emitter

	void SetCameraPosToFollow(float3 position);					//Set Camera position which the billboards will face

	unsigned int GetTextureID(float MaxParticleLife, float time); //This is used by particles update to find which texture of the animation they use

	bool IsEmitterDead() const;									//Usseful if you need to do something else when this emitter dies

private:
	void GenerateMeshResourceBuffers();							//Function to generate new buffers for mesh
	void GenerateUVBuffers();									//When texture rows or columns are updated we update animation UVs
	void GenerateTexturesUVs();									//Generate animation UVs buffers
	void DrawTexturePreview();									//Imgui draw Texture preview + animation edit
	void DrawColorSelector();									//Imgui draw initial/final state editors
	void DrawEmitterOptions();									//Imgui draw emitter options
	bool CreateParticle();										//Function to create a new particle with the stored data

public:
	bool EditorWindowOpen = false;								//Open and clase particle editor window
	KeyInput MouseLeftClick;									//Store mouse left click state. But now it's not used, we use this in previous UI iterations, but left it here if we need it in the future
	float3 CameraPosition = float3::zero;						//Camera position to orientate particles
	bool ShowEmitterBoundBox = false;							//Draw emitter AABB
	bool ShowEmitter = true;									//Draw emitter shape

private:
	ParticleMeshData ParticleMesh;								//Particle mesh to use
	std::list<Particle*> Particles;								//Particles created
	ParticleState InitialState;									//Particle initial state with variable values
	ParticleState FinalState;									//Particle final state with variable values
	ParticleEmitter Emitter;									//Emitter data
	ParticleTextureData TextureData;							//Particle texture to use
	std::vector<float4> TexturesUV_Data;						//Particle animation UVs: UV0 = X-Y		UV1 = Z-W
	//std::vector<float*> TexturesUV_Data_ptr;
	std::vector<unsigned int> TexturesUV_ID;					//Particle animation UVs IDs
	float NextParticleTime = 0.0f;								//Store next time when the next particle will be spawned

	enum
	{
		InitialState_Enum,
		FinalState_Enum
	}ParticleStateEnum = InitialState_Enum;						//Enum to set initial or final state, used by the UI
};

#endif // __ParticleSystem_H__