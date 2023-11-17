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
if (NOT DEFINED QTXMLRPC_LIBRARIES)
  pkg_check_modules(QTXMLRPC REQUIRED Qt${QT_VERSION_MAJOR}XMLRpc>=1.0.0)
endif()

message("Building ${PROJECT_NAME} using Qt${QT_VERSION_MAJOR}")
set(TARGET_NAME "Qt${QT_VERSION_MAJOR}Odoo")

if (Qt${QT_VERSION_MAJOR}Quick_FOUND)
  set(QODOO_CFLAGS "-DQT_QUICK_LIB")
  add_definitions(${QODOO_CFLAGS})
  message("Building ${PROJECT_NAME} with QtQuick support")
endif()

set(source_files
	odoo-qt/QOdoo.cpp
	odoo-qt/QOdooModel.cpp
	odoo-qt/QOdooProduct.cpp
	odoo-qt/QOdooPartner.cpp
	odoo-qt/QOdooCountry.cpp
	odoo-qt/QOdooCountryState.cpp
	odoo-qt/QOdooAccount.cpp
	odoo-qt/QOdooAccountJournal.cpp
	odoo-qt/QOdooAccountGroup.cpp
	odoo-qt/QOdooInvoice.cpp
	odoo-qt/QOdooInvoiceLine.cpp
	odoo-qt/QOdooTax.cpp
	odoo-qt/QOdooCollection.cpp
	odoo-qt/QOdooTableModel.cpp)
set(header_files
	odoo-qt/QOdoo.h
	odoo-qt/QOdooProduct.h
	odoo-qt/QOdooProductTemplate.h
	odoo-qt/QOdooPartner.h
	odoo-qt/QOdooCountry.h
	odoo-qt/QOdooCountryState.h
	odoo-qt/QOdooAccount.h
	odoo-qt/QOdooAccountJournal.h
	odoo-qt/QOdooAccountGroup.h
	odoo-qt/QOdooInvoice.h
	odoo-qt/QOdooInvoiceLine.h
	odoo-qt/QOdooTax.h
	odoo-qt/QOdooCollection.h
	odoo-qt/QOdooModel.h
	odoo-qt/QOdooEnum.h
	odoo-qt/QOdooSearchQuery.h
	odoo-qt/QOdooTableModel.h
  odoo-qt/QOdoo_global.h)

add_library(${TARGET_NAME} SHARED ${source_files} ${header_files})
target_link_libraries(${TARGET_NAME} PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Xml
  ${QTXMLRPC_LIBRARIES}
)

target_compile_definitions(${TARGET_NAME} PRIVATE QTODOO_LIBRARY)

configure_file(QtOdoo.pc.in ${TARGET_NAME}.pc @ONLY)

install(FILES ${header_files} DESTINATION include/odoo-qt)
install(TARGETS ${TARGET_NAME} LIBRARY DESTINATION lib)
install(FILES ${CMAKE_BINARY_DIR}/${TARGET_NAME}.pc DESTINATION lib/pkgconfig)
