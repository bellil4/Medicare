cmake_minimum_required(VERSION 3.16)
project(MedicalConsultationMS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Widgets Sql REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/app.cpp
    src/app.h
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/db/database.cpp
    src/db/database.h
    src/auth/authmanager.cpp
    src/auth/authmanager.h
    src/audit/auditlogger.cpp
    src/audit/auditlogger.h
    src/models/patient.cpp
    src/models/patient.h
    src/repositories/patientrepository.cpp
    src/repositories/patientrepository.h
)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql)
