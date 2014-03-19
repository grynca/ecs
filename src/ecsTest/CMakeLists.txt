include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(ecsTest main.cpp)

# install executable
install(TARGETS ecsTest RUNTIME DESTINATION "bin")
