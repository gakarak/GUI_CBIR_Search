cmake_minimum_required(VERSION 2.6)

#
# проверка Unicode (UTF-8):
# 偉大和強大的俄語。
# اللغة الروسية العظمى والأقوياء.
# Η μεγάλη και πανίσχυρη ρωσική γλώσσα.
# השפה הרוסיה הגדולה והעצומה.
# 大いなるロシア語。
# 위대하고 강력한 러시아어 언어입니다.
# Wielki i potężny język rosyjski.
#

PROJECT(GUI_MVP_Search_v4)

set( TARGET_0 "GUI_MVP_Search_v4" )

set(CMAKE_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    #set(CMAKE_BUILD_TYPE "Debug")
endif()

set(PLATFORM "x32")
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    set(PLATFORM "x64")
endif()

set(BUILD_PREFIX "${CMAKE_BUILD_TYPE}_${PLATFORM}")
message("build prefix=[${BUILD_PREFIX}]")

FIND_PACKAGE( OpenCV REQUIRED )

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else()
    message(STATUS "OpenMP not found!")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

##INCLUDE(${QT_USE_FILE})
##INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_SOURCE_DIR})
##ADD_DEFINITIONS(${QT_DEFINITIONS})

##SET(PRJ_SRC	main.cpp mainwindow.cpp dialogrcpdinfo.cpp dialogimageview.cpp dialogshowpzki.cpp gitempzki.cpp lib_autosim_zki.cpp lib_cpzki.cpp lib_crcpd.cpp lib_img_io.cpp lib_infopzki.cpp polygonitempzki.cpp)
##SET(PRJ_HDR	mainwindow.h dialogrcpdinfo.h dialogimageview.h dialogshowpzki.h gitempzki.h lib_autosim_zki.h lib_cpzki.h lib_crcpd.h lib_img_io.h lib_infopzki.h polygonitempzki.h stdint.h)

SET(PRJ_SRC	main.cpp
		mainwindow.cpp
		draglabel.cpp
		draglistview.cpp
		model2.cpp
		descriptorcalculator.cpp
		workerthread.cpp
		sortedindex.cpp
		workerthreadsorter.cpp)

SET(PRJ_HDR	mainwindow.h
		draglabel.h
		draglistview.h
		model2.h
		descriptorcalculator.h
		workerthread.h
		sortedindex.h
		include/dscplugininterface.h
		workerthreadsorter.h)

SET(PRJ_FORMS	mainwindow.ui )
SET(PRJ_RES	images.qrc)

set( LIB_MVPTREE	"mvptree_mod"         )
##set( LIB_DSCINT		"dscinterface"        )
##set( LIB_DSCCOO		"dsccoomatrix"        )
##set( LIB_DSC		"lib_descriptor2sl"   )

add_library( ${LIB_MVPTREE} STATIC "src/${LIB_MVPTREE}.cpp" "include/${LIB_MVPTREE}.h")
##add_library( ${LIB_DSCINT}  STATIC "src/${LIB_DSCINT}.cpp"  "include/${LIB_DSCINT}.h" )
##add_library( ${LIB_DSCCOO}  STATIC "src/${LIB_DSCCOO}.cpp"  "include/${LIB_DSCCOO}.h" )
##add_library( ${LIB_DSC}     STATIC "src/${LIB_DSC}.cpp"     "include/${LIB_DSC}.h"    )


if(NOT UNIX)
    set( LIB_MMAP "mman" )
    add_library( ${LIB_MMAP} STATIC "src/${LIB_MMAP}.c" "include/${LIB_MMAP}.h")
else()
    set( LIB_MMAP "" )
endif()

##QT4_WRAP_CPP(PRJ_HDR_MOC	${PRJ_HDR})
##QT4_WRAP_UI(PRJ_FORMS_HDR	${PRJ_FORMS})
##QT4_ADD_RESOURCES(PRJ_RES_RCC	${PRJ_RES})

QT5_WRAP_UI(PRJ_FORMS_HDR	${PRJ_FORMS})
QT5_ADD_RESOURCES(PRJ_RES_RCC	${PRJ_RES})


ADD_EXECUTABLE( ${TARGET_0}_${BUILD_PREFIX}		${PRJ_SRC} ${PRJ_HDR_MOC} ${PRJ_FORMS_HDR} ${PRJ_RES_RCC})
TARGET_LINK_LIBRARIES( ${TARGET_0}_${BUILD_PREFIX}
			    ${LIB_MVPTREE} ${LIB_DSCINT} ${LIB_DSCCOO} ${LIB_DSC} ${LIB_MMAP}
			    ${Qt5Widgets_LIBRARIES} ${Qt5Core_LIBRARIES} ${OpenCV_LIBS})


##get_cmake_property(_variableNames VARIABLES)
##foreach (_variableName ${_variableNames})
##   message(STATUS "${_variableName}=${${_variableName}}")
##endforeach()
