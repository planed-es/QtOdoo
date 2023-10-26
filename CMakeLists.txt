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
pkg_check_modules(QTXMLRPC REQUIRED Qt${QT_VERSION_MAJOR}XMLRpc>=1.0.0)

message("Using Qt${QT_VERSION_MAJOR}")
set(TARGET_NAME "Qt${QT_VERSION_MAJOR}Odoo")

if (Qt${QT_VERSION_MAJOR}Quick_FOUND)
  set(QODOO_CFLAGS "-DQT_QUICK_LIB")
  add_definitions(${QODOO_CFLAGS})
  message("Building with QtQuick support")
endif()

set(source_files
	QOdoo.cpp
	QOdooModel.cpp
	QOdooProduct.cpp
	QOdooPartner.cpp
	QOdooCountry.cpp
	QOdooCountryState.cpp
	QOdooAccount.cpp
	QOdooAccountJournal.cpp
	QOdooInvoice.cpp
	QOdooInvoiceLine.cpp
	QOdooTax.cpp
	QOdooCollection.cpp)
set(header_files
	QOdoo.h
	QOdooProduct.h
	QOdooProductTemplate.h
	QOdooPartner.h
	QOdooCountry.h
	QOdooCountryState.h
	QOdooAccount.h
	QOdooAccountJournal.h
	QOdooInvoice.h
	QOdooInvoiceLine.h
	QOdooTax.h
	QOdooCollection.h
	QOdooModel.h
	QOdooEnum.h
	QOdooSearchQuery.h)

add_library(${TARGET_NAME} SHARED ${source_files} ${header_files})
target_link_libraries(${TARGET_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml
  ${QTXMLRPC_LIBRARIES}
)

target_compile_definitions(${TARGET_NAME} PRIVATE QTODOO_LIBRARY)

configure_file(QtOdoo.pc.in ${TARGET_NAME}.pc @ONLY)

install(FILES ${header_files} DESTINATION include)
install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib)
install(FILES ${CMAKE_BINARY_DIR}/${TARGET_NAME}.pc DESTINATION lib/pkgconfig)
