cmake_minimum_required(VERSION 3.14)

project(QtOdoo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Xml REQUIRED)

set(source_files QOdoo.cpp QOdooProduct.cpp QOdooPartner.cpp QOdooInvoice.cpp QOdooInvoiceLine.cpp)
set(header_files QOdoo.h   QOdooProduct.h   QOdooPartner.h   QOdooInvoice.h   QOdooInvoiceLine.h)

add_library(QtOdoo SHARED ${source_files} ${header_files})
target_link_libraries(QtOdoo PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml
  QtXMLRpc)

target_compile_definitions(QtOdoo PRIVATE QTODOO_LIBRARY)

configure_file(QtOdoo.pc.in QtOdoo.pc @ONLY)

install(FILES ${header_files} DESTINATION include)
install(TARGETS QtOdoo LIBRARY DESTINATION lib)
install(FILES ${CMAKE_BINARY_DIR}/QtOdoo.pc DESTINATION lib/pkgconfig)
