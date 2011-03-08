set(SORE_RENDERER_SRCS
    renderer/sore_aggregatebuffermanager.cpp
    renderer/sore_batch.cpp
    renderer/sore_batchingbuffermanager.cpp
    renderer/sore_buffermanager.cpp
    renderer/sore_fbo.cpp
    renderer/sore_gl_command_list.cpp
    renderer/sore_geometrychunk.cpp renderer/sore_geometryprovider.cpp
    renderer/sore_graphics.cpp renderer/sore_graphicsarray.cpp
    renderer/sore_pipe.cpp
    renderer/sore_pipeline_renderer.cpp
    renderer/sore_renderable.cpp renderer/sore_renderer.cpp
    renderer/sore_renderer_std.cpp renderer/sore_renderstate.cpp 
    renderer/sore_glslshader.cpp renderer/sore_simplebuffermanager.cpp
    renderer/sore_sprite.cpp
    renderer/sore_texture.cpp renderer/sore_texturestate.cpp
    renderer/sore_uniformstate.cpp renderer/sore_vbo.cpp
    renderer/sore_vertexarray.cpp
    PARENT_SCOPE)

set(SORE_RENDERER_HEADERS
    renderer/sore_aggregatebuffermanager.h
    renderer/sore_allgl.h renderer/sore_batch.h
    renderer/sore_batchingbuffermanager.h
    renderer/sore_buffermanager.h
    renderer/sore_fbo.h
    renderer/sore_gl_command_list.h
    renderer/sore_geometrychunk.h
    renderer/sore_geometryprovider.h renderer/sore_graphics.h
    renderer/sore_graphicsarray.h
    renderer/sore_pipe.h
    renderer/sore_pipeline_renderer.h renderer/sore_renderable.h
    renderer/sore_renderer.h renderer/sore_renderer_std.h
    renderer/sore_renderstate.h renderer/sore_glslshader.h
    renderer/sore_simplebuffermanager.h
    renderer/sore_sprite.h renderer/sore_texture.h
    renderer/sore_texturestate.h renderer/sore_uniformstate.h
    renderer/sore_vbo.h renderer/sore_vertexarray.h
    PARENT_SCOPE)
