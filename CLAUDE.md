# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Communication Guidelines

**Mentor Mode**: Act as a mentor guiding the user's learning process.

- **Provide Direction Only**: When the user presents a problem, provide only conceptual guidance and direction for solving it.
- **No Code Examples**: Do NOT show code examples unless explicitly requested by the user.
- **Learning First**: The user wants to learn by implementing solutions themselves. Help them understand the approach, not just copy-paste solutions.
- **Answer Format**:
  - Explain the concept
  - Describe the approach/strategy
  - Break down the steps logically
  - Only provide actual code when user asks "코드 보여줘" or similar explicit requests

## Project Overview

Use Korean to communicate with users

DirectX11 game engine built with a component-based architecture. The solution contains:
- **Engine** (DLL): Core rendering and game engine systems
- **Client** (EXE): Game application that consumes the engine
- **Maptool** (EXE): Terrain/map editing tool

## Build System

### Building the Project

1. Open `DX11.sln` in Visual Studio 2017 or later
2. Build the Engine project first (it produces `Engine.dll` and `Engine.lib`)
3. Run `UpdateLib.bat` to copy build artifacts:
   ```batch
   UpdateLib.bat
   ```
   This copies:
   - `Engine.dll` → `Client/Bin/`
   - `Engine.lib` → `EngineSDK/Lib/`
   - Third-party libs → `EngineSDK/Lib/`
   - Public headers → `EngineSDK/Inc/`

4. Build Client project (links against EngineSDK)

### Build Configurations
- Platforms: Win32, x64
- Configurations: Debug, Release
- Output directory: `../Bin/`

### Important: After Engine Changes
Always run `UpdateLib.bat` after modifying Engine code to sync the DLL/headers to Client and EngineSDK directories.

## Architecture Overview

### Core Singleton Managers (Engine Subsystems)

All accessed through `CGameInstance`, which acts as a facade:

```
CGameInstance (Central Coordinator)
├── CGraphic_Device     - DirectX 11 device/context, swap chain
├── CInput_Device       - DirectInput keyboard/mouse
├── CTimer_Manager      - Frame timing and delta time
├── CLevel_Manager      - Scene/level management
├── CPrototype_Manager  - Resource caching (prototype pattern)
├── CObject_Manager     - GameObject lifecycle per level
├── CRenderer           - Render queue (PRIORITY, NONBLEND, BLEND, UI)
├── CPipeLine           - View/projection matrices, camera data
└── CLight_Manager      - Scene lighting
```

### Component-Based Object System

Everything inherits from `CBase` (manual reference counting):

```
CGameObject (Abstract)
├── Has map<wstring, CComponent*> for components
├── Always includes CTransform
├── Add components via Add_Component() from prototypes
└── Override: Initialize_Prototype(), Initialize(), Update_*(), Render()

CComponent Types:
├── CTransform       - Position, rotation, scale, movement
├── CVIBuffer        - Vertex/index buffer (abstract)
│   ├── CMesh        - Individual mesh from model
│   ├── CVIBuffer_Rect    - Simple quad
│   └── CVIBuffer_Terrain - Heightmap terrain
├── CShader          - FX effect file wrapper
├── CTexture         - Texture array management
├── CModel           - Assimp model loader with materials
└── CMaterial        - Material with textures (internal to Engine)
```

### Prototype Pattern for Resources

The engine uses a prototype/clone pattern for all resources:

1. Create original once: `Prototype_Manager::Add_Prototype(level, tag, original)`
2. Clone many times: `Prototype_Manager::Clone_GameObject(level, tag)`
3. Supports per-level isolation (resources cleared when level changes)

Example:
```cpp
// In loading phase
GameInstance->Add_Prototype(LEVEL_STATIC, L"Proto_Shader_VtxNorTex",
    CShader::Create(device, L"ShaderFiles/Shader_VtxNorTex.hlsl"));

// In object creation
pGameObject->Add_Component(LEVEL_STATIC, L"Proto_Shader_VtxNorTex",
    L"Com_Shader");
```

### Frame Update Order

**Update Loop** (`CMainApp::Update()` → `GameInstance::Update_Engine()`):
1. `CInput_Device::Update()` - Poll input
2. `CObject_Manager::Update_Priority()` - Cameras, priority objects
3. `CPipeLine::Update()` - Recalculate inverse matrices
4. `CObject_Manager::Update()` - Regular game objects
5. `CObject_Manager::Update_Late()` - Post-update (collision, positioning)
6. `CLevel_Manager::Update_Level()` - Level-specific logic

**Render Loop** (`CMainApp::Render()` → `GameInstance::Draw()`):
1. Clear back buffer and depth/stencil
2. `CRenderer::Draw()` - Execute render queue in order
   - PRIORITY → NONBLEND → BLEND → UI
3. `CLevel_Manager::Render_Level()` - Level-specific rendering
4. Present swap chain

### Render Queue System

Objects don't render themselves immediately. Instead:
1. In `GameObject::Update_Late()`, call `Renderer->Add_RenderObject(GROUP, this)`
2. Renderer stores pointer in appropriate queue
3. During `Renderer::Draw()`, calls `Render()` on all queued objects in order
4. Queues cleared each frame

Groups: `PRIORITY` (important), `NONBLEND` (opaque), `BLEND` (transparent), `UI`

### Level and Layer System

```
CLevel (Abstract base for scenes)
├── Organized by CLayers (groupings like "Player", "Monster", "Environment")
├── Each Layer contains list<CGameObject*>
└── Add objects via: GameInstance->Add_GameObject_To_Layer(protoLevel, protoTag, layerLevel, layerTag)

Level IDs (Client):
STATIC  - Resources shared across all levels
LOADING - Loading screen
LOGO    - Startup/logo screen
GAMEPLAY - Main game
```

Objects added to `STATIC` level persist across level changes.

### Model and Material Loading

**CModel Component** (uses Assimp):
- Load via `CModel::Create(device, context, MODEL::NONANIM or MODEL::ANIM, filepath)`
- Automatically loads all meshes and materials from file
- Each mesh stored as `CMesh` (CVIBuffer with VTXMESH format)
- Materials stored as `CMaterial` with embedded textures

**CMaterial** (internal, not exported via DLL):
- Wraps `aiMaterial` from Assimp
- Loads textures from model's directory
- Call `Bind_Textures()` to bind all material textures to shader

**Rendering a model**:
```cpp
// In GameObject::Render()
for (each mesh in model) {
    shader->Begin(passIndex);
    model->Bind_Material(meshIndex, shader);  // Binds textures
    model->Render(meshIndex);                 // Draw geometry
}
```

### Vertex Formats

```cpp
VTXPOSTEX  - Position(3) + Texcoord(2)
VTXNORTEX  - Position(3) + Normal(3) + Texcoord(2)
VTXMESH    - Position(3) + Normal(3) + Tangent(3) + Binormal(3) + Texcoord(2)
```

Match vertex format to shader input layout.

### Shader System

**Loading shaders**:
- Uses Effects11 framework (.fx files)
- Store in `Client/Bin/ShaderFiles/`
- Load via `CShader::Create(device, filepath)`

**Binding resources to shaders**:
```cpp
shader->Bind_Matrix("g_WorldMatrix", worldMatrix);
shader->Bind_RawValue("g_vCameraPos", &camPos, sizeof(float4));
shader->Bind_SRV("g_DiffuseTexture", textureSRV);
shader->Bind_SRVs("g_Textures", textureSRVArray, count);
shader->Begin(passIndex);  // Apply pass
vBuffer->Render();         // Draw
```

### Memory Management

**Reference Counting**:
- All objects inherit from `CBase` with manual refcounting
- Use `Safe_AddRef(ptr)` and `Safe_Release(ptr)` macros
- Objects self-delete when refcount reaches 0
- **Critical**: Always `AddRef()` when storing pointers, `Release()` when done

**Level-Based Cleanup**:
- Prototypes stored per-level
- Call `Clear(levelID)` on managers to free level resources
- `STATIC` level persists across level changes

### Transform System

**CTransform Component**:
- Stores position, rotation (quaternion), scale
- Provides movement methods: `Go_Straight()`, `Go_Backward()`, `Turn()`, etc.
- Updates world matrix each frame
- Bind to shaders via `Bind_ShaderResource(shader, "g_WorldMatrix")`

**Matrix States**:
```cpp
enum STATE { RIGHT, UP, LOOK, POSITION }  // Transform matrix rows
Get/Set via: Get_State(STATE), Set_State(STATE, vector)
```

### Pipeline and Camera

**CPipeLine** (singleton):
- Stores VIEW and PROJ matrices from active camera
- Updates inverse matrices each frame (for lighting calculations)
- Camera position available for shader binding
- Access via: `PipeLine->Get_Transform(D3DTS::VIEW)`, etc.

**Camera Setup**:
1. Create camera GameObject (inherit from CCamera)
2. In camera's `Update()`:
   ```cpp
   pTransform->Set_State(POSITION, camPosition);
   // Build view matrix from transform
   PipeLine->Set_Transform(D3DTS::VIEW, viewMatrix);
   PipeLine->Set_Transform(D3DTS::PROJ, projMatrix);
   ```
3. Add camera to PRIORITY render group for early update

### Adding New GameObjects

1. **Create GameObject class** (in Client):
   ```cpp
   class CMyObject : public CGameObject {
       HRESULT Initialize_Prototype() override;
       HRESULT Initialize(void* pArg) override;
       void Update(_float fTimeDelta) override;
       void Update_Late(_float fTimeDelta) override;
       HRESULT Render() override;
   };
   ```

2. **Add components in Initialize()**:
   ```cpp
   Add_Component(LEVEL_STATIC, L"Proto_Shader", L"Com_Shader");
   Add_Component(LEVEL_STATIC, L"Proto_VIBuffer_Rect", L"Com_VIBuffer");
   ```

3. **Register as prototype** (in CLoader or level setup):
   ```cpp
   GameInstance->Add_Prototype(LEVEL_GAMEPLAY, L"Proto_MyObject",
       CMyObject::Create(device, context));
   ```

4. **Spawn instances**:
   ```cpp
   GameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, L"Proto_MyObject",
       LEVEL_GAMEPLAY, L"Layer_MyObjects", &initData);
   ```

### Adding New Component Types

1. Inherit from `CComponent`
2. Implement `Clone()` to create copies
3. Implement `Free()` for cleanup
4. Add constructor with `COMPONENTID` (TYPE_TEXTURE, TYPE_VIBUFFER, etc.)
5. Register as prototype in loader

### Terrain System

**CVIBuffer_Terrain**:
- Loads heightmap from image file
- Generates vertex grid based on image resolution
- Each pixel's brightness = vertex height
- UVs tiled for texture repetition

**CTerrain Object**:
- Uses CVIBuffer_Terrain for geometry
- Typically uses multiple textures with mask for blending
- Shader blends textures based on mask (red/green/blue channels)

### Input Handling

**CInput_Device** (singleton via GameInstance):
```cpp
// Keyboard
if (Input->Get_DIKeyState(DIK_W) & 0x80) { /* W key pressed */ }

// Mouse buttons
if (Input->Get_DIMouseState(MOUSEKEYSTATE::LB) & 0x80) { /* Left click */ }

// Mouse movement (delta)
long deltaX = Input->Get_DIMouseMove(MOUSEMOVESTATE::X);
```

### Light System

**CLight_Manager** (singleton):
- Supports directional and point lights
- Add lights via `GameInstance->Add_Light(device, context, LIGHT_DESC)`
- Lights automatically bound to shaders during render
- Light data includes: direction, position, diffuse, ambient, specular, range

### Threading (CLoader)

Loading uses separate thread:
```cpp
CLoader::Create(device, context, LEVELID)  // Spawns thread
IsFinished()  // Check if loading complete
GetLoadingMessage()  // Get current loading status
```

Critical section protected with `mutex` for thread-safe access.

## Common Workflows

### Creating a New Level

1. Create `CLevel_MyLevel : public CLevel`
2. Implement `Initialize()` to set up level
3. Create `CLoader` function to load resources on thread
4. In loader:
   - Add prototypes for GameObjects
   - Add prototypes for Components (shaders, buffers, textures)
5. In level Initialize():
   - Spawn GameObjects to layers
   - Set up cameras, lights, etc.
6. Transition via: `GameInstance->Open_Level(LEVELID, newLevel)`

### Loading a Model

```cpp
// 1. Add model prototype (usually in CLoader)
GameInstance->Add_Prototype(level, L"Proto_MyModel",
    CModel::Create(device, context, MODEL::NONANIM,
    L"../Bin/Resources/Models/MyModel.fbx"));

// 2. In GameObject, add model component
CModel* pModel = (CModel*)Add_Component(level, L"Proto_MyModel", L"Com_Model");

// 3. Render in GameObject::Render()
for (int i = 0; i < pModel->Get_NumMeshes(); ++i) {
    pModel->Bind_Material(i, pShader);
    pModel->Render(i);
}
```

### Creating Custom Shaders

1. Create `.hlsl` file in `Client/Bin/ShaderFiles/`
2. Define structures matching vertex format
3. Load in GameObject via shader prototype
4. Bind world/view/proj matrices and resources before each draw

Example shader structure:
```hlsl
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;

struct VS_IN { float3 vPosition : POSITION; /* ... */ };
struct VS_OUT { float4 vPosition : SV_POSITION; /* ... */ };
```

## Third-Party Libraries

Located in `Engine/ThirdPartyLib/` and headers in `Engine/Public/`:

- **Effects11** - FX shader framework
- **Assimp** - Model importing (FBX, OBJ, 3DS, etc.)
- **DirectXTK** - Texture loaders, helpers
- **DirectXTex** - Advanced texture processing
- **DirectInput** - Input devices

Link against corresponding `.lib` files in EngineSDK/Lib/.

## Key Macros and Helpers

```cpp
DECLARE_SINGLETON(ClassName)  // In class declaration
IMPLEMENT_SINGLETON(ClassName)  // In .cpp file

Safe_AddRef(ptr)    // Increment refcount
Safe_Release(ptr)   // Decrement refcount, nullify if 0

MSG_BOX(message)    // Debug message box
```

## File Locations

- **Shaders**: `Client/Bin/ShaderFiles/`
- **Models**: `Client/Bin/Resources/Models/`
- **Fonts**: `Client/Bin/Resources/Fonts/`
- **Engine DLL**: `Client/Bin/Engine.dll`
- **Engine SDK**: `EngineSDK/Inc/` (headers), `EngineSDK/Lib/` (libraries)

## Important Conventions

1. **Unicode**: Project uses wide strings (`_tchar`, `wstring`)
2. **Naming**: `C` prefix for classes, `m_` prefix for members, `p` prefix for pointers
3. **Export/Import**: Engine exports via `ENGINE_DLL` macro (dllexport/dllimport)
4. **Type Safety**: Use engine typedefs (`_float`, `_uint`, `_matrix`, etc.)
5. **Math**: Uses DirectXMath (XMVECTOR, XMMATRIX) - note SIMD alignment requirements
6. **Resource Paths**: Always relative to executable location (`../Bin/Resources/`)

## Debugging

- Check refcounts if crashes occur on cleanup
- Verify `UpdateLib.bat` ran after Engine changes
- Ensure vertex format matches shader input layout
- Check that prototypes exist before cloning
- Verify proper AddRef/Release pairing
- Use MSG_BOX for quick debug output
