cmake_minimum_required(VERSION 3.16)
project(MedicalConsultationSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

# Source files
set(SOURCES
    src/main.cpp
    src/app.cpp
    src/db/database.cpp
    src/audit/auditlogger.cpp
    src/auth/authmanager.cpp
    src/models/patient.cpp
    src/repositories/patientrepository.cpp
    src/ui/mainwindow.cpp
)

# Header files
set(HEADERS
    src/app.h
    src/db/database.h
    src/audit/auditlogger.h
    src/auth/authmanager.h
    src/models/patient.h
    src/repositories/patientrepository.h
    src/ui/mainwindow.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt6 libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
