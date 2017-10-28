# Functions

| Function *optional | comment|
-|-
| rendertargetid createrendertarget(width,height,layers,colors,type,minfilter*,magfilter*)   | |
| clear(r*,g*,b*,a*) | can be 0 1 3 or 4 variables set|
| beginpass(rendertargetid*) | no render target set will result rendering to the backbuffer |
| endpass()  | |
| imageid loadimage(filename)  | |
| meshid loadmesh(filename) | |
| shaderid loadshader(vertfile,fragfile,geomfile,controlfile,evalfile)  | 0 on unused shader files |
| drawmesh(meshid,submesh*) | will draw all submeshes if none are given |
| bindattribute(attribute_name,MESH_FLAG) | see MESH_FLAG_* |
| resetattributes() | |
| bindshader(shaderid) | -1 unbinds all shaders |
| setuniformf(name,x,y*,z*,w*) | |
| setuniformi(name,x,y*,z*,w*) | |
| setuniformui(name,x,y*,z*,w*) | |
| depthtest(enable) | 0 1 |
| culling(cullmode) | see CULL_* flags |
| float vec3dot(vec3, vec3) | |
| vec3 vec3normalize(vec3) | |

# Objects

vec3={x:0,y:0,z:0}

# Variables
| Dynamic variables | comment |
-|-
| RENDER_WIDTH | updated with beginpass |
| RENDER_HEIGHT | updated with beginpass |
| TIME | time since glsltool starts|
| DELTA_TIME | time since last loop start | 

| Mesh flags |
---
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
---
| CULL_FRONT |
| CULL_BACK |
| CULL_NONE |
| CULL_BOTH |

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
### texture filters
GL_NEAREST
GL_LINEAR
GL_NEAREST_MIPMAP_NEAREST
GL_LINEAR_MIPMAP_NEAREST
GL_NEAREST_MIPMAP_LINEAR
GL_LINEAR_MIPMAP_LINEAR
