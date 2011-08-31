CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Create some packages

# ix widget style
SET(
  IX_STYLE_SRCS
  ix.json

  Fonts/ix/LiberationSans-Regular.ttf

  Textures/ix/arrow.tga
  Textures/ix/checkbox.tga
  Textures/ix/checkbox_checked.tga
  Textures/ix/button_sheet_active.tga
  Textures/ix/button_sheet_hover.tga
  Textures/ix/button_sheet_normal.tga
  Textures/ix/caret.tga
  Textures/ix/dropdown_bg.tga
  Textures/ix/frame.tga
  Textures/ix/slider_bg.tga
  Textures/ix/slider.tga
  Textures/ix/textfield.tga)

SET(OUTPUT_NAME ix_style.sdp)

ADD_CUSTOM_COMMAND(
	OUTPUT  ${OUTPUT_NAME}
	COMMAND sore_create_package --prefix=data --compression ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME} ${IX_STYLE_SRCS}
	DEPENDS sore_create_package ${IX_STYLE_SRCS}
	WORKING_DIRECTORY ${SORE_ENGINE_SOURCE_DIR}/data)

ADD_CUSTOM_TARGET(ix_style ALL DEPENDS  ${OUTPUT_NAME})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME} DESTINATION .)

# default resources - proxies for shaders, textures, fonts
SET(
  DEFAULT_RESOURCES_SRCS 
  Fonts/default.ttf

  Shaders/default.shad 
  Shaders/default_fragment.glsl
  Shaders/default_vertex.glsl

  Shaders/point_sprite.shad 
  Shaders/point_sprite_fragment.glsl
  Shaders/point_sprite_vertex.glsl

  Shaders/untextured.shad
  Shaders/untextured_fragment.glsl
  Shaders/untextured_vertex.glsl

  Textures/default.tga)

SET(OUTPUT_NAME default_resources.sdp)

ADD_CUSTOM_COMMAND(
	OUTPUT ${OUTPUT_NAME}
	COMMAND sore_create_package --prefix=data --compression ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME} ${DEFAULT_RESOURCES_SRCS}
	DEPENDS sore_create_package ${DEFAULT_RESOURCES_SRCS}
	WORKING_DIRECTORY ${SORE_ENGINE_SOURCE_DIR}/data)

ADD_CUSTOM_TARGET(default_resources ALL DEPENDS ${OUTPUT_NAME})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME} DESTINATION .)