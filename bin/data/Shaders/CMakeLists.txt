CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INSTALL(FILES particles.shad
  particles_v.glsl
  particles_f.glsl
  point_sprite.shad
  point_v.glsl
  point_f.glsl
  untextured.shad
  untextured_fragment.glsl
  untextured_vertex.glsl
  DESTINATION data/Shaders
)