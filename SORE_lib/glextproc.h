
#include "sore_allgl.h"

typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );

// ---------------------------------------------------------------------------
// WGL extensions
// ---------------------------------------------------------------------------
GLEXT_PROC( PFNWGLSWAPINTERVALFARPROC,          wglSwapIntervalEXT          );

#ifdef WIN32
// ---------------------------------------------------------------------------
// OpenGL 1.2 extensions
// ---------------------------------------------------------------------------

GLEXT_PROC( PFNGLBLENDCOLORPROC,                glBlendColor                );
GLEXT_PROC( PFNGLBLENDEQUATIONPROC,             glBlendEquation             );
GLEXT_PROC( PFNGLDRAWRANGEELEMENTSPROC,         glDrawRangeElements         );
GLEXT_PROC( PFNGLCOLORTABLEPROC,                glColorTable                );
GLEXT_PROC( PFNGLCOLORTABLEPARAMETERFVPROC,     glColorTableParameterfv     );
GLEXT_PROC( PFNGLCOLORTABLEPARAMETERIVPROC,     glColorTableParameteriv     );
GLEXT_PROC( PFNGLCOPYCOLORTABLEPROC,            glCopyColorTable            );
GLEXT_PROC( PFNGLGETCOLORTABLEPROC,             glGetColorTable             );
GLEXT_PROC( PFNGLGETCOLORTABLEPARAMETERFVPROC,  glGetColorTableParameterfv  );
GLEXT_PROC( PFNGLGETCOLORTABLEPARAMETERIVPROC,  glGetColorTableParameteriv  );
GLEXT_PROC( PFNGLCOLORSUBTABLEPROC,             glColorSubTable             );
GLEXT_PROC( PFNGLCOPYCOLORSUBTABLEPROC,         glCopyColorSubTable         );
GLEXT_PROC( PFNGLCONVOLUTIONFILTER1DPROC,       glConvolutionFilter1D       );
GLEXT_PROC( PFNGLCONVOLUTIONFILTER2DPROC,       glConvolutionFilter2D       );
GLEXT_PROC( PFNGLCONVOLUTIONPARAMETERFPROC,     glConvolutionParameterf     );
GLEXT_PROC( PFNGLCONVOLUTIONPARAMETERFVPROC,    glConvolutionParameterfv    );
GLEXT_PROC( PFNGLCONVOLUTIONPARAMETERIPROC,     glConvolutionParameteri     );
GLEXT_PROC( PFNGLCONVOLUTIONPARAMETERIVPROC,    glConvolutionParameteriv    );
GLEXT_PROC( PFNGLCOPYCONVOLUTIONFILTER1DPROC,   glCopyConvolutionFilter1D   );
GLEXT_PROC( PFNGLCOPYCONVOLUTIONFILTER2DPROC,   glCopyConvolutionFilter2D   );
GLEXT_PROC( PFNGLGETCONVOLUTIONFILTERPROC,      glGetConvolutionFilter      );
GLEXT_PROC( PFNGLGETCONVOLUTIONPARAMETERFVPROC, glGetConvolutionParameterfv );
GLEXT_PROC( PFNGLGETCONVOLUTIONPARAMETERIVPROC, glGetConvolutionParameteriv );
GLEXT_PROC( PFNGLGETSEPARABLEFILTERPROC,        glGetSeparableFilter        );
GLEXT_PROC( PFNGLSEPARABLEFILTER2DPROC,         glSeparableFilter2D         );
GLEXT_PROC( PFNGLGETHISTOGRAMPROC,              glGetHistogram              );
GLEXT_PROC( PFNGLGETHISTOGRAMPARAMETERFVPROC,   glGetHistogramParameterfv   );
GLEXT_PROC( PFNGLGETHISTOGRAMPARAMETERIVPROC,   glGetHistogramParameteriv   );
GLEXT_PROC( PFNGLGETMINMAXPROC,                 glGetMinmax                 );
GLEXT_PROC( PFNGLGETMINMAXPARAMETERFVPROC,      glGetMinmaxParameterfv      );
GLEXT_PROC( PFNGLGETMINMAXPARAMETERIVPROC,      glGetMinmaxParameteriv      );
GLEXT_PROC( PFNGLHISTOGRAMPROC,                 glHistogram                 );
GLEXT_PROC( PFNGLMINMAXPROC,                    glMinmax                    );
GLEXT_PROC( PFNGLRESETHISTOGRAMPROC,            glResetHistogram            );
GLEXT_PROC( PFNGLRESETMINMAXPROC,               glResetMinmax               );
GLEXT_PROC( PFNGLTEXIMAGE3DPROC,                glTexImage3D                );
GLEXT_PROC( PFNGLTEXSUBIMAGE3DPROC,             glTexSubImage3D             );
GLEXT_PROC( PFNGLCOPYTEXSUBIMAGE3DPROC,         glCopyTexSubImage3D         );

// ---------------------------------------------------------------------------
// OpenGL 1.3 extensions
// ---------------------------------------------------------------------------

GLEXT_PROC( PFNGLACTIVETEXTUREPROC,             glActiveTexture             );
GLEXT_PROC( PFNGLCLIENTACTIVETEXTUREPROC,       glClientActiveTexture       );
GLEXT_PROC( PFNGLMULTITEXCOORD1DPROC,           glMultiTexCoord1d           );
GLEXT_PROC( PFNGLMULTITEXCOORD1DVPROC,          glMultiTexCoord1dv          );
GLEXT_PROC( PFNGLMULTITEXCOORD1FPROC,           glMultiTexCoord1f           );
GLEXT_PROC( PFNGLMULTITEXCOORD1FVPROC,          glMultiTexCoord1fv          );
GLEXT_PROC( PFNGLMULTITEXCOORD1IPROC,           glMultiTexCoord1i           );
GLEXT_PROC( PFNGLMULTITEXCOORD1IVPROC,          glMultiTexCoord1iv          );
GLEXT_PROC( PFNGLMULTITEXCOORD1SPROC,           glMultiTexCoord1s           );
GLEXT_PROC( PFNGLMULTITEXCOORD1SVPROC,          glMultiTexCoord1sv          );
GLEXT_PROC( PFNGLMULTITEXCOORD2DPROC,           glMultiTexCoord2d           );
GLEXT_PROC( PFNGLMULTITEXCOORD2DVPROC,          glMultiTexCoord2dv          );
GLEXT_PROC( PFNGLMULTITEXCOORD2FPROC,           glMultiTexCoord2f           );
GLEXT_PROC( PFNGLMULTITEXCOORD2FVPROC,          glMultiTexCoord2fv          );
GLEXT_PROC( PFNGLMULTITEXCOORD2IPROC,           glMultiTexCoord2i           );
GLEXT_PROC( PFNGLMULTITEXCOORD2IVPROC,          glMultiTexCoord2iv          );
GLEXT_PROC( PFNGLMULTITEXCOORD2SPROC,           glMultiTexCoord2s           );
GLEXT_PROC( PFNGLMULTITEXCOORD2SVPROC,          glMultiTexCoord2sv          );
GLEXT_PROC( PFNGLMULTITEXCOORD3DPROC,           glMultiTexCoord3d           );
GLEXT_PROC( PFNGLMULTITEXCOORD3DVPROC,          glMultiTexCoord3dv          );
GLEXT_PROC( PFNGLMULTITEXCOORD3FPROC,           glMultiTexCoord3f           );
GLEXT_PROC( PFNGLMULTITEXCOORD3FVPROC,          glMultiTexCoord3fv          );
GLEXT_PROC( PFNGLMULTITEXCOORD3IPROC,           glMultiTexCoord3i           );
GLEXT_PROC( PFNGLMULTITEXCOORD3IVPROC,          glMultiTexCoord3iv          );
GLEXT_PROC( PFNGLMULTITEXCOORD3SPROC,           glMultiTexCoord3s           );
GLEXT_PROC( PFNGLMULTITEXCOORD3SVPROC,          glMultiTexCoord3sv          );
GLEXT_PROC( PFNGLMULTITEXCOORD4DPROC,           glMultiTexCoord4d           );
GLEXT_PROC( PFNGLMULTITEXCOORD4DVPROC,          glMultiTexCoord4dv          );
GLEXT_PROC( PFNGLMULTITEXCOORD4FPROC,           glMultiTexCoord4f           );
GLEXT_PROC( PFNGLMULTITEXCOORD4FVPROC,          glMultiTexCoord4fv          );
GLEXT_PROC( PFNGLMULTITEXCOORD4IPROC,           glMultiTexCoord4i           );
GLEXT_PROC( PFNGLMULTITEXCOORD4IVPROC,          glMultiTexCoord4iv          );
GLEXT_PROC( PFNGLMULTITEXCOORD4SPROC,           glMultiTexCoord4s           );
GLEXT_PROC( PFNGLMULTITEXCOORD4SVPROC,          glMultiTexCoord4sv          );
GLEXT_PROC( PFNGLLOADTRANSPOSEMATRIXFPROC,      glLoadTransposeMatrixf      );
GLEXT_PROC( PFNGLLOADTRANSPOSEMATRIXDPROC,      glLoadTransposeMatrixd      );
GLEXT_PROC( PFNGLMULTTRANSPOSEMATRIXFPROC,      glMultTransposeMatrixf      );
GLEXT_PROC( PFNGLMULTTRANSPOSEMATRIXDPROC,      glMultTransposeMatrixd      );
GLEXT_PROC( PFNGLSAMPLECOVERAGEPROC,            glSampleCoverage            );
GLEXT_PROC( PFNGLCOMPRESSEDTEXIMAGE3DPROC,      glCompressedTexImage3D      );
GLEXT_PROC( PFNGLCOMPRESSEDTEXIMAGE2DPROC,      glCompressedTexImage2D      );
GLEXT_PROC( PFNGLCOMPRESSEDTEXIMAGE1DPROC,      glCompressedTexImage1D      );
GLEXT_PROC( PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC,   glCompressedTexSubImage3D   );
GLEXT_PROC( PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC,   glCompressedTexSubImage2D   );
GLEXT_PROC( PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC,   glCompressedTexSubImage1D   );
GLEXT_PROC( PFNGLGETCOMPRESSEDTEXIMAGEPROC,     glGetCompressedTexImage     );

// ---------------------------------------------------------------------------
// OpenGL 1.4 extensions
// ---------------------------------------------------------------------------

GLEXT_PROC( PFNGLBLENDFUNCSEPARATEPROC,         glBlendFuncSeparate         );
GLEXT_PROC( PFNGLFOGCOORDFPROC,                 glFogCoordf                 );
GLEXT_PROC( PFNGLFOGCOORDFVPROC,                glFogCoordfv                );
GLEXT_PROC( PFNGLFOGCOORDDPROC,                 glFogCoordd                 );
GLEXT_PROC( PFNGLFOGCOORDDVPROC,                glFogCoorddv                );
GLEXT_PROC( PFNGLFOGCOORDPOINTERPROC,           glFogCoordPointer           );
GLEXT_PROC( PFNGLMULTIDRAWARRAYSPROC,           glMultiDrawArrays           );
GLEXT_PROC( PFNGLMULTIDRAWELEMENTSPROC,         glMultiDrawElements         );
GLEXT_PROC( PFNGLPOINTPARAMETERFPROC,           glPointParameterf           );
GLEXT_PROC( PFNGLPOINTPARAMETERFVPROC,          glPointParameterfv          );
GLEXT_PROC( PFNGLPOINTPARAMETERIPROC,           glPointParameteri           );
GLEXT_PROC( PFNGLPOINTPARAMETERIVPROC,          glPointParameteriv          );
GLEXT_PROC( PFNGLSECONDARYCOLOR3BPROC,          glSecondaryColor3b          );
GLEXT_PROC( PFNGLSECONDARYCOLOR3BVPROC,         glSecondaryColor3bv         );
GLEXT_PROC( PFNGLSECONDARYCOLOR3DPROC,          glSecondaryColor3d          );
GLEXT_PROC( PFNGLSECONDARYCOLOR3DVPROC,         glSecondaryColor3dv         );
GLEXT_PROC( PFNGLSECONDARYCOLOR3FPROC,          glSecondaryColor3f          );
GLEXT_PROC( PFNGLSECONDARYCOLOR3FVPROC,         glSecondaryColor3fv         );
GLEXT_PROC( PFNGLSECONDARYCOLOR3IPROC,          glSecondaryColor3i          );
GLEXT_PROC( PFNGLSECONDARYCOLOR3IVPROC,         glSecondaryColor3iv         );
GLEXT_PROC( PFNGLSECONDARYCOLOR3SPROC,          glSecondaryColor3s          );
GLEXT_PROC( PFNGLSECONDARYCOLOR3SVPROC,         glSecondaryColor3sv         );
GLEXT_PROC( PFNGLSECONDARYCOLOR3UBPROC,         glSecondaryColor3ub         );
GLEXT_PROC( PFNGLSECONDARYCOLOR3UBVPROC,        glSecondaryColor3ubv        );
GLEXT_PROC( PFNGLSECONDARYCOLOR3UIPROC,         glSecondaryColor3ui         );
GLEXT_PROC( PFNGLSECONDARYCOLOR3UIVPROC,        glSecondaryColor3uiv        );
GLEXT_PROC( PFNGLSECONDARYCOLOR3USPROC,         glSecondaryColor3us         );
GLEXT_PROC( PFNGLSECONDARYCOLOR3USVPROC,        glSecondaryColor3usv        );
GLEXT_PROC( PFNGLSECONDARYCOLORPOINTERPROC,     glSecondaryColorPointer     );
GLEXT_PROC( PFNGLWINDOWPOS2DPROC,               glWindowPos2d               );
GLEXT_PROC( PFNGLWINDOWPOS2DVPROC,              glWindowPos2dv              );
GLEXT_PROC( PFNGLWINDOWPOS2FPROC,               glWindowPos2f               );
GLEXT_PROC( PFNGLWINDOWPOS2FVPROC,              glWindowPos2fv              );
GLEXT_PROC( PFNGLWINDOWPOS2IPROC,               glWindowPos2i               );
GLEXT_PROC( PFNGLWINDOWPOS2IVPROC,              glWindowPos2iv              );
GLEXT_PROC( PFNGLWINDOWPOS2SPROC,               glWindowPos2s               );
GLEXT_PROC( PFNGLWINDOWPOS2SVPROC,              glWindowPos2sv              );
GLEXT_PROC( PFNGLWINDOWPOS3DPROC,               glWindowPos3d               );
GLEXT_PROC( PFNGLWINDOWPOS3DVPROC,              glWindowPos3dv              );
GLEXT_PROC( PFNGLWINDOWPOS3FPROC,               glWindowPos3f               );
GLEXT_PROC( PFNGLWINDOWPOS3FVPROC,              glWindowPos3fv              );
GLEXT_PROC( PFNGLWINDOWPOS3IPROC,               glWindowPos3i               );
GLEXT_PROC( PFNGLWINDOWPOS3IVPROC,              glWindowPos3iv              );
GLEXT_PROC( PFNGLWINDOWPOS3SPROC,               glWindowPos3s               );
GLEXT_PROC( PFNGLWINDOWPOS3SVPROC,              glWindowPos3sv              );

#endif

// ---------------------------------------------------------------------------
// OpenGL 1.5 extensions
// ---------------------------------------------------------------------------

GLEXT_PROC( PFNGLGENQUERIESPROC,                glGenQueries                );
GLEXT_PROC( PFNGLDELETEQUERIESPROC,             glDeleteQueries             );
GLEXT_PROC( PFNGLISQUERYPROC,                   glIsQuery                   );
GLEXT_PROC( PFNGLBEGINQUERYPROC,                glBeginQuery                );
GLEXT_PROC( PFNGLENDQUERYPROC,                  glEndQuery                  );
GLEXT_PROC( PFNGLGETQUERYIVPROC,                glGetQueryiv                );
GLEXT_PROC( PFNGLGETQUERYOBJECTIVPROC,          glGetQueryObjectiv          );
GLEXT_PROC( PFNGLGETQUERYOBJECTUIVPROC,         glGetQueryObjectuiv         );
GLEXT_PROC( PFNGLBINDBUFFERPROC,                glBindBuffer                );
GLEXT_PROC( PFNGLDELETEBUFFERSPROC,             glDeleteBuffers             );
GLEXT_PROC( PFNGLGENBUFFERSPROC,                glGenBuffers                );
GLEXT_PROC( PFNGLISBUFFERPROC,                  glIsBuffer                  );
GLEXT_PROC( PFNGLBUFFERDATAPROC,                glBufferData                );
GLEXT_PROC( PFNGLBUFFERSUBDATAPROC,             glBufferSubData             );
GLEXT_PROC( PFNGLGETBUFFERSUBDATAPROC,          glGetBufferSubData          );
GLEXT_PROC( PFNGLMAPBUFFERPROC,                 glMapBuffer                 );
GLEXT_PROC( PFNGLUNMAPBUFFERPROC,               glUnmapBuffer               );
GLEXT_PROC( PFNGLGETBUFFERPARAMETERIVPROC,      glGetBufferParameteriv      );
GLEXT_PROC( PFNGLGETBUFFERPOINTERVPROC,         glGetBufferPointerv         );

// ---------------------------------------------------------------------------
// OpenGL 2.0 extensions
// ---------------------------------------------------------------------------

GLEXT_PROC( PFNGLBLENDEQUATIONSEPARATEPROC,     glBlendEquationSeparate     );
GLEXT_PROC( PFNGLDRAWBUFFERSPROC,               glDrawBuffers               );
GLEXT_PROC( PFNGLSTENCILOPSEPARATEPROC,         glStencilOpSeparate         );
GLEXT_PROC( PFNGLSTENCILFUNCSEPARATEPROC,       glStencilFuncSeparate       );
GLEXT_PROC( PFNGLSTENCILMASKSEPARATEPROC,       glStencilMaskSeparate       );
GLEXT_PROC( PFNGLATTACHSHADERPROC,              glAttachShader              );
GLEXT_PROC( PFNGLBINDATTRIBLOCATIONPROC,        glBindAttribLocation        );
GLEXT_PROC( PFNGLCOMPILESHADERPROC,             glCompileShader             );
GLEXT_PROC( PFNGLCREATEPROGRAMPROC,             glCreateProgram             );
GLEXT_PROC( PFNGLCREATESHADERPROC,              glCreateShader              );
GLEXT_PROC( PFNGLDELETEPROGRAMPROC,             glDeleteProgram             );
GLEXT_PROC( PFNGLDELETESHADERPROC,              glDeleteShader              );
GLEXT_PROC( PFNGLDETACHSHADERPROC,              glDetachShader              );
GLEXT_PROC( PFNGLDISABLEVERTEXATTRIBARRAYPROC,  glDisableVertexAttribArray  );
GLEXT_PROC( PFNGLENABLEVERTEXATTRIBARRAYPROC,   glEnableVertexAttribArray   );
GLEXT_PROC( PFNGLGETACTIVEATTRIBPROC,           glGetActiveAttrib           );
GLEXT_PROC( PFNGLGETACTIVEUNIFORMPROC,          glGetActiveUniform          );
GLEXT_PROC( PFNGLGETATTACHEDSHADERSPROC,        glGetAttachedShaders        );
GLEXT_PROC( PFNGLGETATTRIBLOCATIONPROC,         glGetAttribLocation         );
GLEXT_PROC( PFNGLGETPROGRAMIVPROC,              glGetProgramiv              );
GLEXT_PROC( PFNGLGETPROGRAMINFOLOGPROC,         glGetProgramInfoLog         );
GLEXT_PROC( PFNGLGETSHADERIVPROC,               glGetShaderiv               );
GLEXT_PROC( PFNGLGETSHADERINFOLOGPROC,          glGetShaderInfoLog          );
GLEXT_PROC( PFNGLGETSHADERSOURCEPROC,           glGetShaderSource           );
GLEXT_PROC( PFNGLGETUNIFORMLOCATIONPROC,        glGetUniformLocation        );
GLEXT_PROC( PFNGLGETUNIFORMFVPROC,              glGetUniformfv              );
GLEXT_PROC( PFNGLGETUNIFORMIVPROC,              glGetUniformiv              );
GLEXT_PROC( PFNGLGETVERTEXATTRIBDVPROC,         glGetVertexAttribdv         );
GLEXT_PROC( PFNGLGETVERTEXATTRIBFVPROC,         glGetVertexAttribfv         );
GLEXT_PROC( PFNGLGETVERTEXATTRIBIVPROC,         glGetVertexAttribiv         );
GLEXT_PROC( PFNGLGETVERTEXATTRIBPOINTERVPROC,   glGetVertexAttribPointerv   );
GLEXT_PROC( PFNGLISPROGRAMPROC,                 glIsProgram                 );
GLEXT_PROC( PFNGLISSHADERPROC,                  glIsShader                  );
GLEXT_PROC( PFNGLLINKPROGRAMPROC,               glLinkProgram               );
GLEXT_PROC( PFNGLSHADERSOURCEPROC,              glShaderSource              );
GLEXT_PROC( PFNGLUSEPROGRAMPROC,                glUseProgram                );
GLEXT_PROC( PFNGLUNIFORM1FPROC,                 glUniform1f                 );
GLEXT_PROC( PFNGLUNIFORM2FPROC,                 glUniform2f                 );
GLEXT_PROC( PFNGLUNIFORM3FPROC,                 glUniform3f                 );
GLEXT_PROC( PFNGLUNIFORM4FPROC,                 glUniform4f                 );
GLEXT_PROC( PFNGLUNIFORM1IPROC,                 glUniform1i                 );
GLEXT_PROC( PFNGLUNIFORM2IPROC,                 glUniform2i                 );
GLEXT_PROC( PFNGLUNIFORM3IPROC,                 glUniform3i                 );
GLEXT_PROC( PFNGLUNIFORM4IPROC,                 glUniform4i                 );
GLEXT_PROC( PFNGLUNIFORM1FVPROC,                glUniform1fv                );
GLEXT_PROC( PFNGLUNIFORM2FVPROC,                glUniform2fv                );
GLEXT_PROC( PFNGLUNIFORM3FVPROC,                glUniform3fv                );
GLEXT_PROC( PFNGLUNIFORM4FVPROC,                glUniform4fv                );
GLEXT_PROC( PFNGLUNIFORM1IVPROC,                glUniform1iv                );
GLEXT_PROC( PFNGLUNIFORM2IVPROC,                glUniform2iv                );
GLEXT_PROC( PFNGLUNIFORM3IVPROC,                glUniform3iv                );
GLEXT_PROC( PFNGLUNIFORM4IVPROC,                glUniform4iv                );
GLEXT_PROC( PFNGLUNIFORMMATRIX2FVPROC,          glUniformMatrix2fv          );
GLEXT_PROC( PFNGLUNIFORMMATRIX3FVPROC,          glUniformMatrix3fv          );
GLEXT_PROC( PFNGLUNIFORMMATRIX4FVPROC,          glUniformMatrix4fv          );
GLEXT_PROC( PFNGLVALIDATEPROGRAMPROC,           glValidateProgram           );
GLEXT_PROC( PFNGLVERTEXATTRIB1DPROC,            glVertexAttrib1d            );
GLEXT_PROC( PFNGLVERTEXATTRIB1DVPROC,           glVertexAttrib1dv           );
GLEXT_PROC( PFNGLVERTEXATTRIB1FPROC,            glVertexAttrib1f            );
GLEXT_PROC( PFNGLVERTEXATTRIB1FVPROC,           glVertexAttrib1fv           );
GLEXT_PROC( PFNGLVERTEXATTRIB1SPROC,            glVertexAttrib1s            );
GLEXT_PROC( PFNGLVERTEXATTRIB1SVPROC,           glVertexAttrib1sv           );
GLEXT_PROC( PFNGLVERTEXATTRIB2DPROC,            glVertexAttrib2d            );
GLEXT_PROC( PFNGLVERTEXATTRIB2DVPROC,           glVertexAttrib2dv           );
GLEXT_PROC( PFNGLVERTEXATTRIB2FPROC,            glVertexAttrib2f            );
GLEXT_PROC( PFNGLVERTEXATTRIB2FVPROC,           glVertexAttrib2fv           );
GLEXT_PROC( PFNGLVERTEXATTRIB2SPROC,            glVertexAttrib2s            );
GLEXT_PROC( PFNGLVERTEXATTRIB2SVPROC,           glVertexAttrib2sv           );
GLEXT_PROC( PFNGLVERTEXATTRIB3DPROC,            glVertexAttrib3d            );
GLEXT_PROC( PFNGLVERTEXATTRIB3DVPROC,           glVertexAttrib3dv           );
GLEXT_PROC( PFNGLVERTEXATTRIB3FPROC,            glVertexAttrib3f            );
GLEXT_PROC( PFNGLVERTEXATTRIB3FVPROC,           glVertexAttrib3fv           );
GLEXT_PROC( PFNGLVERTEXATTRIB3SPROC,            glVertexAttrib3s            );
GLEXT_PROC( PFNGLVERTEXATTRIB3SVPROC,           glVertexAttrib3sv           );
GLEXT_PROC( PFNGLVERTEXATTRIB4NBVPROC,          glVertexAttrib4Nbv          );
GLEXT_PROC( PFNGLVERTEXATTRIB4NIVPROC,          glVertexAttrib4Niv          );
GLEXT_PROC( PFNGLVERTEXATTRIB4NSVPROC,          glVertexAttrib4Nsv          );
GLEXT_PROC( PFNGLVERTEXATTRIB4NUBPROC,          glVertexAttrib4Nub          );
GLEXT_PROC( PFNGLVERTEXATTRIB4NUBVPROC,         glVertexAttrib4Nubv         );
GLEXT_PROC( PFNGLVERTEXATTRIB4NUIVPROC,         glVertexAttrib4Nuiv         );
GLEXT_PROC( PFNGLVERTEXATTRIB4NUSVPROC,         glVertexAttrib4Nusv         );
GLEXT_PROC( PFNGLVERTEXATTRIB4BVPROC,           glVertexAttrib4bv           );
GLEXT_PROC( PFNGLVERTEXATTRIB4DPROC,            glVertexAttrib4d            );
GLEXT_PROC( PFNGLVERTEXATTRIB4DVPROC,           glVertexAttrib4dv           );
GLEXT_PROC( PFNGLVERTEXATTRIB4FPROC,            glVertexAttrib4f            );
GLEXT_PROC( PFNGLVERTEXATTRIB4FVPROC,           glVertexAttrib4fv           );
GLEXT_PROC( PFNGLVERTEXATTRIB4IVPROC,           glVertexAttrib4iv           );
GLEXT_PROC( PFNGLVERTEXATTRIB4SPROC,            glVertexAttrib4s            );
GLEXT_PROC( PFNGLVERTEXATTRIB4SVPROC,           glVertexAttrib4sv           );
GLEXT_PROC( PFNGLVERTEXATTRIB4UBVPROC,          glVertexAttrib4ubv          );
GLEXT_PROC( PFNGLVERTEXATTRIB4UIVPROC,          glVertexAttrib4uiv          );
GLEXT_PROC( PFNGLVERTEXATTRIB4USVPROC,          glVertexAttrib4usv          );
GLEXT_PROC( PFNGLVERTEXATTRIBPOINTERPROC,       glVertexAttribPointer       );

// ---------------------------------------------------------------------------
// We obtain backwards compatibility with OpenGL < 2.0 by using the ARB extensions
// for shaders
// ---------------------------------------------------------------------------

GLEXT_PROC( PFNGLATTACHSHADERPROC,              glAttachObjectARB              );
GLEXT_PROC( PFNGLBINDATTRIBLOCATIONPROC,        glBindAttribLocationARB        );
GLEXT_PROC( PFNGLCOMPILESHADERPROC,             glCompileShaderARB             );
GLEXT_PROC( PFNGLCREATEPROGRAMPROC,             glCreateProgramARB             );
GLEXT_PROC( PFNGLCREATESHADERPROC,              glCreateShaderARB              );
GLEXT_PROC( PFNGLDELETEPROGRAMPROC,             glDeleteProgramARB             );
GLEXT_PROC( PFNGLDELETESHADERPROC,              glDeleteShaderARB              );
GLEXT_PROC( PFNGLDETACHSHADERPROC,              glDetachShaderARB              );
GLEXT_PROC( PFNGLDISABLEVERTEXATTRIBARRAYPROC,  glDisableVertexAttribArrayARB  );
GLEXT_PROC( PFNGLENABLEVERTEXATTRIBARRAYPROC,   glEnableVertexAttribArrayARB   );
GLEXT_PROC( PFNGLGETACTIVEATTRIBPROC,           glGetActiveAttribARB           );
GLEXT_PROC( PFNGLGETACTIVEUNIFORMPROC,          glGetActiveUniformARB          );
GLEXT_PROC( PFNGLGETATTACHEDSHADERSPROC,        glGetAttachedShadersARB        );
GLEXT_PROC( PFNGLGETATTRIBLOCATIONPROC,         glGetAttribLocationARB         );
GLEXT_PROC( PFNGLGETPROGRAMIVPROC,              glGetProgramivARB              );
GLEXT_PROC( PFNGLGETPROGRAMINFOLOGPROC,         glGetProgramInfoLogARB         );
GLEXT_PROC( PFNGLGETSHADERIVPROC,               glGetShaderivARB               );
GLEXT_PROC( PFNGLGETSHADERINFOLOGPROC,          glGetShaderInfoLogARB          );
GLEXT_PROC( PFNGLGETSHADERSOURCEPROC,           glGetShaderSourceARB           );
GLEXT_PROC( PFNGLGETUNIFORMLOCATIONPROC,        glGetUniformLocationARB        );
GLEXT_PROC( PFNGLGETUNIFORMFVPROC,              glGetUniformfvARB              );
GLEXT_PROC( PFNGLGETUNIFORMIVPROC,              glGetUniformivARB              );
GLEXT_PROC( PFNGLGETVERTEXATTRIBDVPROC,         glGetVertexAttribdvARB         );
GLEXT_PROC( PFNGLGETVERTEXATTRIBFVPROC,         glGetVertexAttribfvARB         );
GLEXT_PROC( PFNGLGETVERTEXATTRIBIVPROC,         glGetVertexAttribivARB         );
GLEXT_PROC( PFNGLGETVERTEXATTRIBPOINTERVPROC,   glGetVertexAttribPointervARB   );
GLEXT_PROC( PFNGLISPROGRAMPROC,                 glIsProgramARB                 );
GLEXT_PROC( PFNGLISSHADERPROC,                  glIsShaderARB                  );
GLEXT_PROC( PFNGLLINKPROGRAMPROC,               glLinkProgramARB               );
GLEXT_PROC( PFNGLSHADERSOURCEPROC,              glShaderSourceARB              );
GLEXT_PROC( PFNGLUSEPROGRAMPROC,                glUseProgramObjectARB          );
GLEXT_PROC( PFNGLUNIFORM1FPROC,                 glUniform1fARB                 );
GLEXT_PROC( PFNGLUNIFORM2FPROC,                 glUniform2fARB                 );
GLEXT_PROC( PFNGLUNIFORM3FPROC,                 glUniform3fARB                 );
GLEXT_PROC( PFNGLUNIFORM4FPROC,                 glUniform4fARB                 );
GLEXT_PROC( PFNGLUNIFORM1IPROC,                 glUniform1iARB                 );
GLEXT_PROC( PFNGLUNIFORM2IPROC,                 glUniform2iARB                 );
GLEXT_PROC( PFNGLUNIFORM3IPROC,                 glUniform3iARB                 );
GLEXT_PROC( PFNGLUNIFORM4IPROC,                 glUniform4iARB                 );
GLEXT_PROC( PFNGLUNIFORM1FVPROC,                glUniform1fvARB                );
GLEXT_PROC( PFNGLUNIFORM2FVPROC,                glUniform2fvARB                );
GLEXT_PROC( PFNGLUNIFORM3FVPROC,                glUniform3fvARB                );
GLEXT_PROC( PFNGLUNIFORM4FVPROC,                glUniform4fvARB                );
GLEXT_PROC( PFNGLUNIFORM1IVPROC,                glUniform1ivARB                );
GLEXT_PROC( PFNGLUNIFORM2IVPROC,                glUniform2ivARB                );
GLEXT_PROC( PFNGLUNIFORM3IVPROC,                glUniform3ivARB                );
GLEXT_PROC( PFNGLUNIFORM4IVPROC,                glUniform4ivARB                );
GLEXT_PROC( PFNGLUNIFORMMATRIX2FVPROC,          glUniformMatrix2fvARB          );
GLEXT_PROC( PFNGLUNIFORMMATRIX3FVPROC,          glUniformMatrix3fvARB          );
GLEXT_PROC( PFNGLUNIFORMMATRIX4FVPROC,          glUniformMatrix4fvARB          );
GLEXT_PROC( PFNGLVALIDATEPROGRAMPROC,           glValidateProgramARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB1DPROC,            glVertexAttrib1dARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB1DVPROC,           glVertexAttrib1dvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB1FPROC,            glVertexAttrib1fARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB1FVPROC,           glVertexAttrib1fvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB1SPROC,            glVertexAttrib1sARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB1SVPROC,           glVertexAttrib1svARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB2DPROC,            glVertexAttrib2dARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB2DVPROC,           glVertexAttrib2dvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB2FPROC,            glVertexAttrib2fARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB2FVPROC,           glVertexAttrib2fvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB2SPROC,            glVertexAttrib2sARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB2SVPROC,           glVertexAttrib2svARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB3DPROC,            glVertexAttrib3dARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB3DVPROC,           glVertexAttrib3dvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB3FPROC,            glVertexAttrib3fARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB3FVPROC,           glVertexAttrib3fvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB3SPROC,            glVertexAttrib3sARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB3SVPROC,           glVertexAttrib3svARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB4NBVPROC,          glVertexAttrib4NbvARB          );
GLEXT_PROC( PFNGLVERTEXATTRIB4NIVPROC,          glVertexAttrib4NivARB          );
GLEXT_PROC( PFNGLVERTEXATTRIB4NSVPROC,          glVertexAttrib4NsvARB          );
GLEXT_PROC( PFNGLVERTEXATTRIB4NUBPROC,          glVertexAttrib4NubARB          );
GLEXT_PROC( PFNGLVERTEXATTRIB4NUBVPROC,         glVertexAttrib4NubvARB         );
GLEXT_PROC( PFNGLVERTEXATTRIB4NUIVPROC,         glVertexAttrib4NuivARB         );
GLEXT_PROC( PFNGLVERTEXATTRIB4NUSVPROC,         glVertexAttrib4NusvARB         );
GLEXT_PROC( PFNGLVERTEXATTRIB4BVPROC,           glVertexAttrib4bvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB4DPROC,            glVertexAttrib4dARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB4DVPROC,           glVertexAttrib4dvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB4FPROC,            glVertexAttrib4fARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB4FVPROC,           glVertexAttrib4fvARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB4IVPROC,           glVertexAttrib4ivARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB4SPROC,            glVertexAttrib4sARB            );
GLEXT_PROC( PFNGLVERTEXATTRIB4SVPROC,           glVertexAttrib4svARB           );
GLEXT_PROC( PFNGLVERTEXATTRIB4UBVPROC,          glVertexAttrib4ubvARB          );
GLEXT_PROC( PFNGLVERTEXATTRIB4UIVPROC,          glVertexAttrib4uivARB          );
GLEXT_PROC( PFNGLVERTEXATTRIB4USVPROC,          glVertexAttrib4usvARB          );
GLEXT_PROC( PFNGLVERTEXATTRIBPOINTERPROC,       glVertexAttribPointerARB       );

// ---------------------------------------------------------------------------
// OpenGL 2.1 extensions
// ---------------------------------------------------------------------------

//GLEXT_PROC( PFNGLUNIFORMMATRIX2X3FVPROC,        glUniformMatrix2x3fv        );
//GLEXT_PROC( PFNGLUNIFORMMATRIX3X2FVPROC,        glUniformMatrix3x2fv        );
//GLEXT_PROC( PFNGLUNIFORMMATRIX2X4FVPROC,        glUniformMatrix2x4fv        );
//GLEXT_PROC( PFNGLUNIFORMMATRIX4X2FVPROC,        glUniformMatrix4x2fv        );
//GLEXT_PROC( PFNGLUNIFORMMATRIX3X4FVPROC,        glUniformMatrix3x4fv        );
//GLEXT_PROC( PFNGLUNIFORMMATRIX4X3FVPROC,        glUniformMatrix4x3fv        );
