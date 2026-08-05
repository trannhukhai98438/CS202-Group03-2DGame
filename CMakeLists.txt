# Yêu cầu phiên bản CMake tối thiểu
cmake_minimum_required(VERSION 3.14)

# Tên dự án và ngôn ngữ
project(Custom2DPlatformer VERSION 1.0 DESCRIPTION "CS202 OOP Platformer Game" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Đảm bảo rằng SFML được build dưới dạng static library
set(BUILD_SHARED_LIBS OFF)
# 1. KHAI BÁO VÀ TẢI THƯ VIỆN BẰNG FETCHCONTENT
include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1 # Có thể đổi thành phiên bản nhóm muốn sử dụng
)

# Lệnh này sẽ tải mã nguồn và thêm SFML vào quá trình build
FetchContent_MakeAvailable(SFML)

# 2. GOM CÁC FILE SOURCE (.cpp)
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 3. TẠO FILE THỰC THI (EXECUTABLE)
add_executable(${PROJECT_NAME} ${SOURCES})

# 4. CHỈ ĐỊNH THƯ MỤC CHỨA FILE HEADER (.h)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/Entities/Character/Enemy
    ${CMAKE_SOURCE_DIR}/include/Entities/Character/Hero
    ${CMAKE_SOURCE_DIR}/include/Entities/Character/Hero/HeroForm
    ${CMAKE_SOURCE_DIR}/include/Entities/Character/Hero/HeroState
    ${CMAKE_SOURCE_DIR}/include/Entities/Item
    ${CMAKE_SOURCE_DIR}/include/Entities/Block
    ${CMAKE_SOURCE_DIR}/include/Utilities
)

# 5. LINK THƯ VIỆN SFML VÀO PROJECT
# SFML khi được tải bằng FetchContent sẽ giữ các tên target tiêu chuẩn
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# 6. COPY THƯ MỤC ASSETS VÀO THƯ MỤC BUILD
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)