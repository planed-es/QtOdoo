cmake_minimum_required(VERSION 3.14)

project(QtOdoo LANGUAGES CXX VERSION 1.0.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network Xml REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick)
pkg_check_modules(QTXMLRPC REQUIRED QtXMLRpc>=1.0.0)

message("Using Qt${QT_VERSION_MAJOR}")

set(source_files
	QOdoo.cpp
	QOdooModel.cpp
	QOdooProduct.cpp
	QOdooPartner.cpp
	QOdooAccount.cpp
	QOdooInvoice.cpp
	QOdooInvoiceLine.cpp
	QOdooTax.cpp
	QOdooCollection.cpp)
set(header_files
	QOdoo.h
	QOdooProduct.h
	QOdooProductTemplate.h
	QOdooPartner.h
	QOdooAccount.h
	QOdooInvoice.h
	QOdooInvoiceLine.h
	QOdooTax.h
	QOdooCollection.h
	QOdooModel.h
	QOdooEnum.h
	QOdooSearchQuery.h)

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
