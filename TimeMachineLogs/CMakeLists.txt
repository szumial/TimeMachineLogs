cmake_minimum_required(VERSION 3.16)

project(TimeMachineLogs LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(TimeMachineLogs
  main.cpp
  ApplicationConstants.h
  ArchiverModeHelper.h
  FileHasher.h FileHasher.cpp
  Archiver.h Archiver.cpp
  FileEntry.h FileEntry.cpp
  FileCollector.h FileCollector.cpp
)
target_link_libraries(TimeMachineLogs Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS TimeMachineLogs
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
