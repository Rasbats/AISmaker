##---------------------------------------------------------------------------
## Author:      Rasbats aka. Mike Rossiter
## Copyright:
## License:     wxWidgets License
##---------------------------------------------------------------------------

# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2)

PROJECT(AISmaker_pi)

SET(PACKAGE_NAME AISmaker_pi)
SET(VERBOSE_NAME AISmaker)
SET(TITLE_NAME AISmaker)
SET(CPACK_PACKAGE_CONTACT "Mike Rossiter")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")

#SET(CMAKE_BUILD_TYPE Debug)
SET( wxWidgets_USE_DEBUG OFF)
SET( wxWidgets_USE_UNICODE ON)
SET( wxWidgets_USE_UNIVERSAL OFF)
SET( wxWidgets_USE_STATIC OFF)

SET(NAME_SUFFIX "ov50")
SET(CMAKE_CXX_STANDARD 11)

INCLUDE("cmake/PluginConfigure.cmake")

SET(SRC_AISmaker
        src/AISmaker_pi.h
        src/AISmaker_pi.cpp
        src/icons.h
        src/icons.cpp
	    src/AISmakergui.h
	    src/AISmakergui.cpp
	    src/AISmakergui_impl.cpp
	    src/AISmakergui_impl.h
		src/ocpn_plugin.h

	)


ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_AISmaker})

ADD_DEFINITIONS(-DTIXML_USE_STL)

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
