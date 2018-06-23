# Functions

| Function *optional | comment|
|-|-|
| rendertargetid createrendertarget(width,height,layers,components,glformat,relative*)   | components 1-4 or GL_R, G_RGB... if relative is set width and height are float numbers where 1.0 = window size |
| clear(r*,g*,b*,a*) | can be 0 1 3 or 4 variables set|
| beginpass(rendertargetid*) | no render target set will result rendering to the backbuffer |
| endpass()  | |
| imageid loadimage(filename,origin*)  | IMG_TOP, IMG_BOTTOM|
| meshid loadmesh(filename) | |
| meshid generateplane(subdivw*,subdivh*,width*,height*) | defaults to 0 0 2 2 | 
| shaderid loadshader(vertfile,fragfile,geomfile,controlfile,evalfile)  | 0 on unused shader files |
| drawmesh(meshid,submesh*) | will draw all submeshes if none are given |
| bindattribute(attribute_name,MESH_FLAG) | see MESH_FLAG_* |
| bindshader(shaderid) | -1 unbinds all shaders |
| bindtexture(samplername,textureid,minfilter*,magfilter*,clamp_s*,clamp_t*);
| bindrendertarget(samplername,textureid,layer,minfilter*,magfilter*,clamp_s*,clamp_t*);
| setuniformf(name,x,y*,z*,w*) | |
| setuniformi(name,x,y*,z*,w*) | |
| setuniformui(name,x,y*,z*,w*) | |
| setuniformmat4(name,mat4) | |;
| depthtest(enable) | 0 1 |
| culling(cullmode) | see CULL_* flags |
| float vec3dot(vec3, vec3) | |
| vec3 vec3normalize(vec3) | |
| vec3 vec3cross(vec3,vec3) | |
| vec3 vec3add(vec3,vec3) | |
| vec3 vec3sub(vec3,vec3) | |
| vec3 vec3mul(vec3,scalar) | |
| vec3 vec3mat4mul(vec3,mat4) | assumes that w is 1 and will divide by w after multiplying |
| mat4 mat4loadidentity() | |
| mat4 mat4setscale(x,y*,z*) | 1 or 3 variables|
| mat4 mat4settranslation(x,y,z) | |
| mat4 mat4setrotation(angle,x,y,z) | |
| mat4 mat4setperspective(fov,aspect,near,far) | |
| mat4 mat4mul(mat4,mat4) | |
| mat4 mat4transpose(mat4) | |
| mat4 mat4invert(mat4) | |
| setmat4anglemode(mode) | RADIANS or DEGREES |
| sin | |
| cos | |
| tan | |
| asin | |
| acos | |
| atan | |
| floor | |
| ceil | |
| abs | |
| sqrt | |
| debugmode | see DEBUG_*|
| debugstep | sets which renderpass to show (step%renderpasses so its fine to just increase this number forever) | 


# Objects

```javascript
vec3={x:0,y:0,z:0};
mat4={
    m11:0,m12:0,m13:0,m14,
    m21:0,m22:0,m23:0,m24,
    m31:0,m32:0,m33:0,m34,
    m41:0,m42:0,m43:0,m44
};
```

# Variables
| Dynamic variables | comment |
|-|-|
| RENDER_WIDTH | updated with beginpass |
| RENDER_HEIGHT | updated with beginpass |
| WINDOW_WIDTH | updated on loop start |
| WINDOW_HEIGHT | updated on loop start |
| TIME | time since glsltool starts|
| DELTA_TIME | time since last loop start |

| Debug flags |
|---|
| DEBUG_OFF |
| DEBUG_RENDERALLSTEPS |
| DEBUG_RENDERSINGLESTEP |

| Mesh flags |
|---|
| MESH_FLAG_POSITION |
| MESH_FLAG_NORMAL |
| MESH_FLAG_TANGENT |
| MESH_FLAG_BINORMAL |
| MESH_FLAG_TEXCOORD0 |
| MESH_FLAG_TEXCOORD1 |
| MESH_FLAG_TEXCOORD2 |
| MESH_FLAG_TEXCOORD3 |
| MESH_FLAG_TEXCOORD4 |
| MESH_FLAG_TEXCOORD5 |
| MESH_FLAG_TEXCOORD6 |
| MESH_FLAG_TEXCOORD7 |
| MESH_FLAG_COLOR0 |
| MESH_FLAG_COLOR1 |
| MESH_FLAG_COLOR2 |
| MESH_FLAG_COLOR3 |
| MESH_FLAG_COLOR4 |
| MESH_FLAG_COLOR5 |
| MESH_FLAG_COLOR6 |
| MESH_FLAG_COLOR7 |

| Culling flags |
|---|
| CULL_FRONT |
| CULL_BACK |
| CULL_NONE |
| CULL_BOTH |

| Mouse | comment |
|-|-|
| MOUSE_X | |
| MOUSE_Y | |
| MOUSE_DELTA_X | change in mouse position since the last frame |
| MOUSE_DELTA_Y | change in mouse position since the last frame |
| MOUSE_1 | |
| MOUSE_2 | |
| MOUSE_3 | |
| MOUSE_4 | |
| MOUSE_5 | |
| MOUSE_6 | |
| MOUSE_7 | |
| MOUSE_8 | |
| PRESSED | mouse/keyboard button state |
| PRESSED_NOW | is the button pressed this frame |

| Keyboard | comment |
|-|-|
| KEY_SPACE | |
| KEY_APOSTROPHE | |
| KEY_COMMA | |
| KEY_MINUS | |
| KEY_PERIOD | |
| KEY_SLASH | |
| KEY_0 | |
| KEY_1 | |
| KEY_2 | |
| KEY_3 | |
| KEY_4 | |
| KEY_5 | |
| KEY_6 | |
| KEY_7 | |
| KEY_8 | |
| KEY_9 | |
| KEY_SEMICOLON | |
| KEY_EQUAL | |
| KEY_A | |
| KEY_B | |
| KEY_C | |
| KEY_D | |
| KEY_E | |
| KEY_F | |
| KEY_G | |
| KEY_H | |
| KEY_I | |
| KEY_J | |
| KEY_K | |
| KEY_L | |
| KEY_M | |
| KEY_N | |
| KEY_O | |
| KEY_P | |
| KEY_Q | |
| KEY_R | |
| KEY_S | |
| KEY_T | |
| KEY_U | |
| KEY_V | |
| KEY_W | |
| KEY_X | |
| KEY_Y | |
| KEY_Z | |
| KEY_LEFT_BRACKET | |
| KEY_BACKSLASH | |
| KEY_RIGHT_BRACKET | |
| KEY_GRAVE_ACCENT | |
| KEY_WORLD_1 | |
| KEY_WORLD_2 | |
| KEY_ESCAPE | |
| KEY_ENTER | |
| KEY_TAB | |
| KEY_BACKSPACE | |
| KEY_INSERT | |
| KEY_DELETE | |
| KEY_RIGHT | |
| KEY_LEFT | |
| KEY_DOWN | |
| KEY_UP | |
| KEY_PAGE_UP | |
| KEY_PAGE_DOWN | |
| KEY_HOME | |
| KEY_END | |
| KEY_CAPS_LOCK | |
| KEY_SCROLL_LOCK | |
| KEY_NUM_LOCK | |
| KEY_PRINT_SCREEN | |
| KEY_PAUSE | |
| KEY_F1 | |
| KEY_F2 | |
| KEY_F3 | |
| KEY_F4 | |
| KEY_F5 | |
| KEY_F6 | |
| KEY_F7 | |
| KEY_F8 | |
| KEY_F9 | |
| KEY_F10 | |
| KEY_F11 | |
| KEY_F12 | |
| KEY_F13 | |
| KEY_F14 | |
| KEY_F15 | |
| KEY_F16 | |
| KEY_F17 | |
| KEY_F18 | |
| KEY_F19 | |
| KEY_F20 | |
| KEY_F21 | |
| KEY_F22 | |
| KEY_F23 | |
| KEY_F24 | |
| KEY_F25 | |
| KEY_KP_0 | |
| KEY_KP_1 | |
| KEY_KP_2 | |
| KEY_KP_3 | |
| KEY_KP_4 | |
| KEY_KP_5 | |
| KEY_KP_6 | |
| KEY_KP_7 | |
| KEY_KP_8 | |
| KEY_KP_9 | |
| KEY_KP_DECIMAL | |
| KEY_KP_DIVIDE | |
| KEY_KP_MULTIPLY | |
| KEY_KP_SUBTRACT | |
| KEY_KP_ADD | |
| KEY_KP_ENTER | |
| KEY_KP_EQUAL | |
| KEY_LEFT_SHIFT | |
| KEY_LEFT_CONTROL | |
| KEY_LEFT_ALT | |
| KEY_LEFT_SUPER | |
| KEY_RIGHT_SHIFT | |
| KEY_RIGHT_CONTROL | |
| KEY_RIGHT_ALT | |
| KEY_RIGHT_SUPER | |
| KEY_MENU | |

## OpenGL variables

GL_UNSIGNED_BYTE
GL_BYTE
GL_UNSIGNED_SHORT
GL_SHORT
GL_UNSIGNED_INT
GL_INT
GL_FLOAT
GL_UNSIGNED_BYTE_3_3_2
GL_UNSIGNED_BYTE_2_3_3_REV
GL_UNSIGNED_SHORT_5_6_5
GL_UNSIGNED_SHORT_5_6_5_REV
GL_UNSIGNED_SHORT_4_4_4_4
GL_UNSIGNED_SHORT_4_4_4_4_REV
GL_UNSIGNED_SHORT_5_5_5_1
GL_UNSIGNED_SHORT_1_5_5_5_REV
GL_UNSIGNED_INT_8_8_8_8
GL_UNSIGNED_INT_8_8_8_8_REV
GL_UNSIGNED_INT_10_10_10_2
GL_UNSIGNED_INT_2_10_10_10_REV
###  gl formats
GL_RED
GL_RG
GL_RGB
GL_BGR
GL_RGBA
GL_BGRA
GL_RED_INTEGER
GL_RG_INTEGER
GL_RGB_INTEGER
GL_BGR_INTEGER
GL_RGBA_INTEGER
GL_BGRA_INTEGER
GL_STENCIL_INDEX
GL_DEPTH_COMPONENT
GL_DEPTH_STENCIL
GL_R8
GL_R8_SNORM
GL_R16
GL_R16_SNORM
GL_RG8
GL_RG8_SNORM
GL_RG16
GL_RG16_SNORM
GL_R3_G3_B2
GL_RGB4
GL_RGB5
GL_RGB8
GL_RGB8_SNORM
GL_RGB10
GL_RGB12
GL_RGB16_SNORM
GL_RGBA2
GL_RGBA4
GL_RGB5_A1
GL_RGBA8
GL_RGBA8_SNORM
GL_RGB10_A2
GL_RGB10_A2UI
GL_RGBA12
GL_RGBA16
GL_SRGB8
GL_SRGB8_ALPHA8
GL_R16F
GL_RG16F
GL_RGB16F
GL_RGBA16F
GL_R32F
GL_RG32F
GL_RGB32F
GL_RGBA32F
GL_R11F_G11F_B10F
GL_RGB9_E5
GL_R8I
GL_R8UI
GL_R16I
GL_R16UI
GL_R32I
GL_R32UI
GL_RG8I
GL_RG8UI
GL_RG16I
GL_RG16UI
GL_RG32I
GL_RG32UI
GL_RGB8I
GL_RGB8UI
GL_RGB16I
GL_RGB16UI
GL_RGB32I
GL_RGB32UI
GL_RGBA8I
GL_RGBA8UI
GL_RGBA16I
GL_RGBA16UI
GL_RGBA32I
GL_RGBA32UI
### texture filters
GL_NEAREST
GL_LINEAR
GL_NEAREST_MIPMAP_NEAREST
GL_LINEAR_MIPMAP_NEAREST
GL_NEAREST_MIPMAP_LINEAR
GL_LINEAR_MIPMAP_LINEAR
GL_CLAMP_TO_EDGE
GL_MIRRORED_REPEAT
GL_REPEAT
