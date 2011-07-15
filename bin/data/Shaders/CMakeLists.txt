CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INSTALL(FILES 
  particles.shad
  particles_v.glsl
  particles_f.glsl

  particles_shadowmap.shad
  particles_shadowmap_v.glsl
  particles_shadowmap_f.glsl
  
  point_sprite_alphatest.shad
  point_alphatest_v.glsl
  point_alphatest_f.glsl
  
  untextured.shad
  untextured_fragment.glsl
  untextured_vertex.glsl
 
  untextured_lit.shad
  untextured_lit_fragment.glsl
  untextured_lit_vertex.glsl
  
  DESTINATION data/Shaders
)