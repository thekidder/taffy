CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(IX_STYLE_SRCS arrow.tga button_sheet_active.tga button_sheet_hover.tga button_sheet_normal.tga caret.tga dropdown_bg.tga frame.tga slider_bg.tga slider.tga textfield.tga LiberationSans-Regular.ttf)

SET(OUTPUT_NAME ix_style.sdp)

ADD_CUSTOM_COMMAND(
	OUTPUT  ${OUTPUT_NAME}
	COMMAND sore_create_package --prefix=data/ix_style --compression ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME} ${IX_STYLE_SRCS}
	DEPENDS sore_create_package ${IX_STYLE_SRCS}
	WORKING_DIRECTORY ${SORE_ENGINE_SOURCE_DIR}/data/ix_style
)

ADD_CUSTOM_TARGET(ix_style ALL DEPENDS  ${OUTPUT_NAME})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME} DESTINATION .)