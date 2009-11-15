cmake_minimum_required(VERSION 2.6)

set(SORE_GUI_SRCS gui/sore_button.cpp gui/sore_dropdown.cpp
  gui/sore_framewidget.cpp gui/sore_framewindow.cpp
  gui/sore_renderstats.cpp gui/sore_sliderwidget.cpp
  gui/sore_textfield.cpp gui/sore_textwidget.cpp
  gui/sore_topwidget.cpp gui/sore_widget.cpp gui/sore_units.cpp
  PARENT_SCOPE)

set(SORE_GUI_HEADERS gui/sore_button.h
  gui/sore_dropdown.h gui/sore_framewidget.h gui/sore_framewindow.h
  gui/sore_renderstats.g gui/sore_sliderwidget.h gui/sore_textfield.h
  gui/sore_textwidget.h gui/sore_topwidget.h gui/sore_widget.h
  gui/sore_units.h 
  PARENT_SCOPE)