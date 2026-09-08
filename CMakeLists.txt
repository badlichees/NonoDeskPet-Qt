cmake_minimum_required(VERSION 3.16)

project(NonoDeskPet
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appnonodeskpet
    main.cpp
)

qt_add_qml_module(appnonodeskpet
    URI NonoDeskPet
    QML_FILES
        Main.qml
)

target_link_libraries(appnonodeskpet PRIVATE
    Qt6::Quick
)
