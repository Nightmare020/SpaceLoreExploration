cmake_minimum_required(VERSION 3.10)
project(SpaceLoreGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Path to Bullet
set(BULLET_ROOT "${CMAKE_SOURCE_DIR}/bullet3-master")

# Add Bullet as a subdirectory
add_subdirectory(${BULLET_ROOT} bullet_build)

# Add source file
add_executable(Game Game.cpp)

# Link Bullet libraries
target_link_libraries(Game
	${BULLET_ROOT}/src
	${BULLET_ROOT}/src/LinearMath
	${BULLET_ROOT}/src/BulletDynamics
	${BULLET_ROOT}/src/BulletCollision
)

# Include Bullet headers
target_include_directories(Game PRIVATE
	${BULLET_ROOT}/src
)