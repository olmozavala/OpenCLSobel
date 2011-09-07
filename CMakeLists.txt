IF (WIN32)
	FIND_PATH( FREEIMAGE_INCLUDE_PATH FreeImage.h
		${PROJECT_SOURCE_DIR}/extern/FreeImage
		DOC "The directory where FreeImage.h resides")
	FIND_LIBRARY( FREEIMAGE_LIBRARY
		NAMES FreeImage freeimage
		PATHS
		${PROJECT_SOURCE_DIR}/FreeImage
		DOC "The FreeImage library")
ELSE (WIN32)
	FIND_PATH( FREEIMAGE_INCLUDE_PATH FreeImage.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where FreeImage.h resides")
	FIND_LIBRARY( FREEIMAGE_LIBRARY
		NAMES FreeImage freeimage
		PATHS
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The FreeImage library")
ENDIF (WIN32)

SET(FREEIMAGE_LIBRARIES ${FREEIMAGE_LIBRARY})

IF (FREEIMAGE_INCLUDE_PATH AND FREEIMAGE_LIBRARY)
	SET( FREEIMAGE_FOUND TRUE CACHE BOOL "Set to TRUE if GLEW is found, FALSE otherwise")
ELSE (FREEIMAGE_INCLUDE_PATH AND FREEIMAGE_LIBRARY)
	SET( FREEIMAGE_FOUND FALSE CACHE BOOL "Set to TRUE if GLEW is found, FALSE otherwise")
ENDIF (FREEIMAGE_INCLUDE_PATH AND FREEIMAGE_LIBRARY)

MARK_AS_ADVANCED(
	FREEIMAGE_FOUND
	FREEIMAGE_LIBRARY
	FREEIMAGE_LIBRARIES
	FREEIMAGE_INCLUDE_PATH)


message(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${FREEIMAGE_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

add_library(ErrorCodes ErrorCodes.h ErrorCodes.cpp)
add_executable( SobelOZ SobelOZ.cpp )
target_link_libraries( SobelOZ ${OPENCL_LIBRARIES} ${FREEIMAGE_LIBRARIES} ErrorCodes)

configure_file(SobelOZ.cl ${CMAKE_CURRENT_BINARY_DIR}/SobelOZ.cl COPYONLY)
configure_file(test.jpg ${CMAKE_CURRENT_BINARY_DIR}/test.jpg COPYONLY)
