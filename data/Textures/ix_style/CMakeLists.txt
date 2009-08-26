CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_CUSTOM_TARGET(build_ix_style ALL sore_create_package --prefix=data/Textures/ix_style --compression ${CMAKE_BINARY_DIR}/ix_style.sdp arrow.tga button_sheet_active.tga button_sheet_hover.tga button_sheet_normal.tga caret.tga dropdown_bg.tga frame.tga slider_bg.tga slider.tga textfield.tga WORKING_DIRECTORY ${SORE_ENGINE_SOURCE_DIR}/data/Textures/ix_style)

INSTALL(FILES ${CMAKE_BINARY_DIR}/ix_style.sdp DESTINATION .)