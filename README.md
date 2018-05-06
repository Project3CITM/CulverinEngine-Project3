# nJoy Game's Unthroned
## CONTENTS

	0) WHAT IS OUR GAME?
	1) SYSTEM REQUIREMENTS
	2) INSTALLATION
	3) GENERAL INFORMATION
	4) CONFIGURATION
	5) CONTROLS
	6) KNOWN ISSUES
	7) CONTACT INFORMATION
	8) CHANGELOG
	9) TEAM

## 0) WHAT IS OUR GAME?

nJoy Game is an accessible dungeon crawler game based on Game of Thrones TV series universe featuring King's Landing as a playable level. 

You can find more information about our game in our GDD: https://github.com/Project3CITM/CulverinEngine-Project3/wiki/Game-Design-Document

![KingsLanding](https://gyazo.com/2544c62547298f0e347c9d63ce75dd20.jpg)

## 1) SYSTEM REQUIREMENTS

nJoy Game uses and requires Microsoft DirectX 11.0.

### Supported Operating Systems are:

	Windows

### The following operating systems are NOT supported/tested:

	Linux, Mac

### Minimum Hardware Requirements

	1.2 GHz Intel core i3
	1Gb of RAM
	500 MB of free hard disk space
	video card support OpenGL 3.3
	Keyboard
	Mouse
 
## 2) INSTALLATION

Download our latest build at https://github.com/Project3CITM/CulverinEngine-Project3/releases

Just download the release, unpack it, open the NjoyGames_VerticalSlice3 folders and open the NjoyGames_VerticalSlice1.exe file. 

### Save Games:

	Not yet implemented.

## 3) GENERAL INFORMATION

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

### OpenGL:
nJoy Game requires OpenGL 3.3 to run. For the game to run properly, it is necessary to have video drivers that are OpenGL 3.3 compliant.

### Windows 10:
We recommend manufacturer distributed drivers for Windows 10 (i.e. drivers that expose the user controls for changing settings on the particular piece of hardware), instead of the default drivers installed by Microsoft as part of the machine setup process. 

[![Build status](https://ci.appveyor.com/api/projects/status/2e9av5y6o70lle38?svg=true)](https://ci.appveyor.com/project/martimyc/culverinengine-project3)

## 4) CONFIGURATION

### Sound Provider:
We use Wwise for the sound management.
& provided some audios from https://freesound.org/

### Screen Resolution:
For this release, is recommended to not rescale the window, UI not rescaled yet.

## 5) CONTROLS
As a player you can execute the following controls:

* Press E to rotate the camera to the Right.
* Press Q to rotate the camera to the Left.
* Press W to move forward one tile.
* Press S to move backward one tile.
* Press D to move one tile to your Right.
* Press A to move one tile to your Left.
* Press 1  to use primary ability
* Press 2  to use secondary ability
* Press Tab to switch to the left character
* Press R to switch to the right character
* Press 4 to use the right suport ability
* Press 3 to use the left suport ability
* Press SPACE to activate the lever

//Debug

* Press F1 To Put Total GodMode 
* Press F9 To Put GodMode, Habilities without cost
* Press F10 To Put the secundary hability cooldown in 1 seg
* Press F11 to quit damage

## 6) KNOWN ISSUES

https://github.com/Project3CITM/CulverinEngine-Project3/issues

## 7) CONTACT INFORMATION

Web Site: https://github.com/Project3CITM/CulverinEngine-Project3

## 8) CHANGELOG
### Pre Alpha 2 V 0.4
 	* the maps have been changed to 7 different maps
	* New Assets at the stages
	* New boss AI
	* New Transitions between stages
	* Bug Fixing
	

### Alpha V 0.3

	* Radial Filler in Habilities
        * Shielded Enemy Animations improved
        * New Visual Feedback on hp/stamine bars
        * New Particles
	* Bug Fixes


### Pre-Alpha 1 V0.19.3

* Game pad actions improve
	* Bug Fixes

### Pre-Alpha 1 V0.19.2

	* Radial Filler in Habilities
        * Shielded Enemy Animations improved
        * New Visual Feedback on hp/stamine bars
        * New Particles
	* Bug Fixes

### Pre-Alpha 1 V0.19

	* New Map Lvl2 distribution 
	* New 2 Puzzle on lvl 2, (one binari, open the gates )
	* New Enemy life feedback 
	* New Puzzle on lvl 3
	* Bug Fixes 

### Vertical Slice 3 V0.17.1
	* Iddle Camera
	* Dead player animations
	* Animations improve
	* Bug Fixes

### Vertical Slice 3 V0.10

	* 2 Enemy added
	* realistic AI for each of the enemies
	* New Map
	* New Puzzle
 	* New PBR materials
	* New Map Assets
	* Bloom
	* GameLoop
	* New UI Menus & win/lose Windows
	* BUG fixes

### Vertical Slice 2 v0.09:
	* Enemy added	
	* Puzzle completed
	* Lights added

### Vertical Slice 2 v0.08.2:
	* 2 more characters added (Theon and Daenerys)
	* Simple puzzle
	* Simple PBR material tests
	* New UI art

### Vertical Slice 1 v0.08: 
	* Character can Move between tiles and rotate.
	* Character can Attack and block.
	* Simple combat with the AI.
	* AI Pathfinding.
	* Character Arms model and his Animations
	* Character Sword
	* Enemy Model and his Animations
	* Hand-painted Textures
	* Environment Props
	* 1 Level
	* Interactive UI Buttons
	* Healt and Stamina UI representation
	* Level Music and FX such as footsteps, torch fire…


## 9) TEAM

| Member        |Rol           |
| ------------- |:-------------:| 
| Management | | 
| Carlos Peralta Sorolla| Producer| 
| Art | | 
| Sergio Alvarez Llorca |Artist | 
| Gerard Gil Calvo | Artist | 
| Daniel Olóndriz Rosique |Lead Artist | 
| Javier Ariel Ortiz Peirano  | Artist | 
| Guillermo Jesus  Pinto Henriques | Artist | 
| José Antonio Rodriguez Millán | Artist | 
|Marc Samper Tarancón| Artist | 
| Pau Serra Colome| Artist | 
| Daniel Triviño Merida| Artist | 
|Code | | 
| Nicolás Babot Altisén| Programmer |
| Carlos Cabreira Sánchez| Programmer | 
| Josep Casanovas Vidal | Programmer | 
| Adrián Castillo López| Programmer | 
| Marc Fabián Vilà| Programmer | 
| Miquel Izquierdo Cunill| Programmer | 
| Elliot Jiménez Bosch| Programmer | 
|Marc Latorre Freixes| Programmer | 
| Daniel López López| Programmer | 
| Martí Majó Ylla-Català| Programmer | 
|Ferran Martín Vilà | Programmer | 
|Xavier Olivenza Busquets |Lead Programmer | 
| Jordi Oña Rufí| Programmer | 
| Joan Pareja Piñol| Programmer | 
|Pere Rifà Planella | Programmer | 
|Andreu Rojas Barrera| Programmer | 
| Èric Solà Vila | Programmer | 
| Design| | 
| Aleix Cardona Zuecos | Designer| 
| Sebastian Delgado Vargas | Designer | 
|  Adrián Higuero Torres | Designer | 
|Marc López Fàbregas | Lead Designer | 
| Francisco Ruiz Giménez | Designer | 
| Sergio Sáez Calero | Designer | 
| Rubén Gonzalo Sardón Roldán | Designer | 






















































# Culverin 3D Engine
This is a project consistent of creating a Game Engine that allows you to use tools to create video games.

It's created for educational purposes, and we are two students that are studying a Video Games Development and Design degree at CITM, Terrassa (UPC).


Check out our Web: https://github.com/elliotjb/3D-Engine

Download Last Version: https://github.com/elliotjb/3D-Engine/releases/tag/Assignment3 

# VERY IMPORTANT!
Make sure that you store the Release in a path without spaces, because the system won't detect successfully the path, and you won't be able to compile any script.
- Correct Path: "C:/User/Desktop/Culverin_v0.8/..."
- Incorrect Path: "C:/Program Files/Culverin_v0.8/..."

## Example Street scene:
![](https://i.gyazo.com/2fb46188c5188013b7bd76f6e2278bfc.jpg)

# FEATURES
- Configuration options (Application, Memory Consumption, Window, Camera, Input, Audio, Scene, GUI, Renderer, Resources).
- Game Objects Structure (with components & following a hierarchy of parent-childrens).
- Basic GameObject Creation (Cube, Empty object).
- Component options (Select between all loaded meshes(MESH COMPONENT), materials (MATERIAL COMPONENT), reset transforms (TRANSFORM COMPONENT), etc.)
- GameObject/Components deletion.
- Main Camera (Frustum geometry with modificable parameters).
- Game Clock (Play, Pause & Play Frame Modes).
- In Game Mode, you will see the scene through the active camera you have selected from your gameobjects hierarchy.
- Models importation (Imports meshes and textures, and creates Game Objects with correct components, following the correct hierarchy).
- Models reimportation (You can modify an imported mesh/texture externally and changes will be applied automatically in the engine).
- You can select game objects that have a bounding box directly from Scene Screen (or you can select them through hierarchy window).
- Skybox as background of the scene.
- Editable quadtree that will contain static objects. 
- Materials and Meshes components uses Resources to load data only once in memory.
- Scripting System: Unity-like functionality. You have access to the scene gameobjects and act on them (modifying name, transform (pos, rot, scale),
  Enabling/Disabling it, etc.).

# HIGH LEVEL SYSTEM: SCRIPTING SYSTEM.

You can create your own scripts (.cs files), with restricted functionality, and attach them to the GameObjects of the scene.

- By the moment, you can only attach one script per GameObject.
- All the variables of the script are shown in the inspector (for debuging purposes).
- EDITOR OUTPUT VARIABLES: in the inspector, the variables that are recognizable are: INT, FLOAT, STRING, GAMEOBJECT. (other variables will be shown as UNKNOWN TYPE/VALUE).
- You can edit variables from the editor to modify the script.
- Make sure you don't have compilation errors in your scripts (you won't be able to run the "Game").
- You can modify scripts directly from the inspector, by clicking the "< Edit Script >" button. You will see a new window with the code.
  Once you have modified the script, click "File"->"Save".
- Each script has access to the GameObject that it's attached ("GameObject.gameObject").


## HOW TO CREATE A NEW SCRIPT:
 
1) Create-> C# Script.
2) Write the class name to generate the script.  
- WARNING: don't write the same name of an existing script (it won't be generated). 
3) Then, your new script is saved into assets folder, and compiled by default.



## IMPORT EXTERNAL SCRIPTS:

You can drag and drop a script into the engine window to import it. If it has no errors, it will be compiled successfully.

Otherwise you will have to edit it and save changes to recompile it.


## EDIT SCRIPTS:
 
1) To edit it, you have to attach it to a GameObject first: Inspector->Add Component-> Script.
2) You will have to choose between all imported scripts ("Select script...")
3) <Edit Script>: opens a new window with the code will apepear. You can edit it and Save changes (File->Save).


## SCRIPTS IN THE INSPECTOR

- <Edit Script>: open editor window to edit the script.
- As a component, script has a checkbox to activate/deactivate it when entering Play Mode.
- NAME: name of the script.
- VARIABLES: a list with all the variables of the script (TYPE, NAME , VALUE).



## FUNCTIONS TO USE

In your scripts, you can call these specific functions to: 


1) ACT ON GAMEOBJECTS:

- VOID GameObject.gameObject.SetActive(bool): set active/inactive a object.
- BOOL GameObject.gameObject.isActive(): check if it's active/inactive.
- VOID GameObject.gameObject.SetName(string): change name of the object.
- STRING GameObject.gameObject.GetName(): get name of the object.
- STRING GameObject.gameObject.GetComponent<Component>: get component specified of the gameobject (only Transform by now).
- VECTOR3 Transform.Position: position (x, y, z) of the object.
- VECTOR3 Transform.Rotation: rotation [euler] (x, y, z) of the object.
- VECTOR3 Transform.RotateAroundAxis(Vector3): rotate the transform around the specified axis.



2) CONSOLE:
 
- VOID Debug.Log(string): Logs the written message into the engine console.



3) INPUT:
 
- BOOL Input.KeyDown(string)/KeyUp(string)/KeyRepeat(string): check the state of the desired keyboard key.
   
[FIND THE NAME OF THE KEY HERE: https://wiki.libsdl.org/SDL_Scancode]
 
- BOOL Input.MouseButtonDown(int)/MouseButtonUp(int)/MouseButtonRepeat(int): check state of mouse buttons.
   
[1: Left button, 2: Middle button, 3:Right button]



4) TIME:
 
- FLOAT Time.DeltaTime(): return delta time of the game (real dt modified by time scale).



# SCENE SETUP

1) From "Models" folder, drag and drop "tank.obj" into the Engine Window. It will be imported. You can drag folder.
2) Double click it to load into scene. Delete the "g_default" child objects (they are unnecessary).
3) Now, drag and drop from "Scripts" folder the 3 .cs files. They will be compiled successfully.
4) Select "tank.obj" (parent object) and attach it the "Tank" script. 
   You can change the values of rotation speed and movement speed.(Add Component->script, Select script...-> Tank).
5) Select "gTower" child game object and attach it the "myTurret" script. You can change its rotation speed.
6) Select "MainCamera" object and attach it the "CameraController" script.
7) In "target" variable, yot have to set "tank.obj" to follow the tank.
8) Now, you are ready to enjoy! (Hit PLAY button).

9) Video Tutorial Setup: https://youtu.be/tNWcKUdz2rw


- CONTROLS:

	- W/A/S/D: move the tank.
	- Left MouseButton/Right MouseButton: rotate the turret.
	- Spacebar: SHOOT!. (Console message "SHOOT!").


# INNOVATIONS

- We have created our own DLL "CulverinEditor" to load all the assemblies to enable our own c# functions.
- Basic GameObject Creation: Cube.
- Entering Play Mode enables the Active Game Camera to see the scene from it.
- Copy/Paste of GameObjects: by right clicking them (or Ctrl+C and Ctrl+V).
- Skybox as background: you can choose between 2 predefined skyboxes in Windows->Configuration->Scene.
- Resource Management:
	1) You can import an entire folder in the engine to copy all the files inside it, and see them through "Project" Window.
	2) Reimporting elements: you can import a model/texture, edit them externally, and changes will be applied automatically.
	3) Meta files: importing a model/texture will create a .meta file with information about the imported elemend and resources it uses.
	4) Folders Management: you can create, rename (only for empty folders), and remove folders (it will delete resources if they are inside, so, components that use these resources will be "empty").
	5) Resource Manager: window to see all resources loaded in the scene: rename them, see reference counting... and more [Access it via: Windows->Resources].
- Project window like Unity: with resizable icons and same navigation as Unity: folders on the left, files of the folder on the right.
- Gizmos: enable transform modifications of gameobjects in the scene.
- Mouse without restrictions: when you are moving the editor camera with right-click, if you reach a screen border, the mouse will
  reappear on the extreme side of the screen.

# IMPORTING/REIMPORTING FILES

To import a model (.fbx/.obj), texture (.png/.jpg/.tga) or a folder to the Engine, just drag it inside the window.
It will create all necessary resources to apply to GameObjects/Components. 
To load a model into the scene, double-click the icon that has appeared when the file was dragged. It will create a prefab.

Helper videos about resource managing:
- Importing/Reimporting Models: https://youtu.be/r2dZ5sxbU-U
- Importing/Reimporting Textures: https://youtu.be/rUXQ10-Nwls
- Editing Mesh & Material components: https://youtu.be/rsceOFcBO0c

# CAMERA CONTROLS (Unity-like)

- Right Mouse Button: look around with static position
- Left Mouse Button + L-Alt: orbit around a selected object.
- Mouse Wheel: Zoom in / Zoom out.
- Middle Mouse Button: drag and move the camera vertically and horitzontally.
- F: center the Camera on the selected object.

# GIZMOS

Only enabled when Editor Mode is Active.
When you select a game object, its gizmo will be activated, to edit its transform.
Controls:
 - 1: Translate.
 - 2: Rotate.
 - 3: Scale.

If you have problems to select a game object by clicking on the scene screen, tree to move the 
camera to get a better vision angle.

# QUADTREE

In Windows->Configuration->Scene you will see QUADTREE options:
- You can enable/disable its Debug Draw.
- You can change the size of the root node with the slider.
- To implement static objects inside it, first create game objects and set them to 'static'.
  Then, make sure all of them are inside the quadtree boundaries and click "UPDATE QUADTREE" button (only when Editor Mode is active).
- The quadtree will correctly be divided and will contain static objects.

# CAMERA CULLING

- Only enabled in Play Mode.
- To cull static objects you have to set them 'static' first, then update the quadtree. If they are inside the quadtree, culling for
  static objects in Play Mode is enabled.

# GAME OBJECTS / COMPONENT OPTIONS

You can right-click a game object or a component to see which options are enabled that will modify the selected element.
Warning: for this release, cubes created in the engine hasn't got texture coords, so you won't be able to see the applied texture.

# STATIC OBJECTS

When setting an object 'static', you won't be able to modify its transform when Play Mode is activated.

#VERY IMPORTANT!

Make sure that you store the Release in a path without spaces, because the system won't detect successfully the path, and you won't be able to compile any script.

# UPDATES

- 0.8:
    * Embedded Mono in our application.
    * Created the main structure to link c++ functions with c# functions.
    * Resource Script included.
    * Tank scripts created and .obj model added.
    * Code cleaning.
    * Created own assembly "CulverinEditor.dll" to manage c# scripting.
    * Resource Manager Window added.

- 0.7:
    * Added Gizmos that work well with children objects.
    * Implemented Skybox (2 different skyboxes for now).
    * You can update quadtree to collect static games inside it to speed up frustum culling.
    * Added popups to warn users about Object Deletion, Setting Static an object with childs 
      and another PopUp to warn that it's impossible to set more than one game camera active.
    * GameObjects/Components Options when right clicking them.
    * Resource manager with Import/Reimport functionality.
    * Camera moving without restrictions (only with right-click).
    * Correct Save/Load of the scene when entering/exiting Play Mode.
- 0.6:
    * Added Game Clock (with PLAY/PAUSE/PLAY FREAME buttons).
    * Structured camera with a frustum.
    * Added Game camera to visualize the scene in Game Mode.
    * Added Resources Manager (in progress).
    * Implemented Quadtree.
    * Mouse Picking feature added.
    * Camera culling fixed.
    * Added Guizmos to operate with local transforms of Game Objects.
    * Importing models transformations fixed.
    * Implemented Resources.
    * Add class JSONSerialization, Save & Load: Scenes and Prefabs.
    * Fixed Bugs with Import and Load.
- 0.5.5:
	* Fixed Bugs with transmors.
	* Fixed Bugs with Window Project.
- 0.5.4:
	* Add Window Project:
		> In left window, you can see a hierarchy folders.
		> In right, All files from a folder Test.
	* Add Modules -> Importer and Loader
		> Import and Load Meshes.
		> Import Textures.
	* Add ModuleFs.
	* GameObject & Components Structure:
		> Components: transform, mesh, material and camera.
	*Hierarchy and Inspector Windows work on Game Objects structure.
- 0.5:
	* Implemented diference movement Camera (Now is Like Unity).
	* Add more modules in Configuration Window
	* We can set different textures on each different mesh of the model (if imported correctly).
	* Color of the model can be edited.
	* Debug info 
	* Bounding box, Enable/Disable the model.
- 0.4.1:
	* Add buttom to Revert Style Docking. (v0.4.1.1)
	* Added Basic GameObject - Component Structure (Like Unity Engine)
	* Reestructured Model Importer (Now loads the texture it contains in his data automatically).
	* Implemented Save and Load Doking windows.
	* Render Scene in a window "ImGui".

- 0.4:
	* Add ImGui::Dock
	* Add window Hierarchy and Inspector
	* Add icons (play, pause stop).
	* Drag & Drop funcionality (to import FBX models).
	* Texture 2D implemented.
- 0.3:
	* Add icon .exe (v0.3.2.1)
	* Implemented Assimp (v0.3.2)  
	* Fixed structure Objects (v0.3.2)
	* Add window exit (Save, Don't Save and Cancel) - "(v0.3.1)"
	* Implemented "Render" options inside "Configuration" window
		> Depth test, lightning, color material, wireframe,
		  axis, smooth polygons and fog mode.
	* "Hardware" Window implementation:
		> OpenGL info.
	* Creation of spheres and boxes (in "Creation" Window).
- 0.2:
	* Implemented "Configuration" Window:
		> Frame rate options.
		> Window options (resizable, fullscreen, etc).
		> Volume adjoustment.
	* Added "About us..." Window:
		> Name, description, libraries used.
	* "Hardware" Window:
		> CPU's, GPU descriptions & info.

- 0.1.1:
	* Implemented Delete Objects in Scene

# Libraries Used:

(Not created by us!)
- MathGeoLib -> http://clb.demon.fi/MathGeoLib/nightly/ 
- ImGui -> https://github.com/ocornut/imgui 
- Bullet -> http://bulletphysics.org 
- Sdl -> http://www.libsdl.org/
- parson (json parser) -> https://github.com/kgabis/parson
- ASSIMP -> http://assimp.sourceforge.net/
- DevIL -> http://openil.sourceforge.net/
- Mono -> http://www.mono-project.com/

# Authors: 
- Elliot Jimenez: https://github.com/elliotjb
- Jordi Oña: https://github.com/Jordior97

Repository Link: https://github.com/elliotjb/3D-Engine
