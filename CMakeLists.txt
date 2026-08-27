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
# Dùng CONFIGURE_DEPENDS để CMake tự động nhận diện khi có file .cpp mới được tạo
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# 3. TẠO FILE THỰC THI (EXECUTABLE)
add_executable(${PROJECT_NAME} ${SOURCES})

# 4. CHỈ ĐỊNH THƯ MỤC CHỨA FILE HEADER (.h)
# Dùng CONFIGURE_DEPENDS để tự động quét các thư mục con khi có file .h mới
file(GLOB_RECURSE INC_ALL_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/include/*.h" "${CMAKE_SOURCE_DIR}/include/*.hpp")
set(INC_DIRS ${CMAKE_SOURCE_DIR}/include)
foreach(f ${INC_ALL_FILES})
    get_filename_component(dir ${f} DIRECTORY)
    list(APPEND INC_DIRS ${dir})
endforeach()
list(REMOVE_DUPLICATES INC_DIRS)

target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIRS})

# 5. LINK THƯ VIỆN SFML VÀO PROJECT
# SFML khi được tải bằng FetchContent sẽ giữ các tên target tiêu chuẩn
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# 6. COPY THƯ MỤC ASSETS VÀO THƯ MỤC BUILD
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

# SFML audio is linked statically, but its OpenAL backend remains a runtime
# DLL on Windows. Keep it beside the executable for both Debug and Release.
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SFML_OPENAL_ARCH x64)
    else()
        set(SFML_OPENAL_ARCH x86)
    endif()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SFML_SOURCE_DIR}/extlibs/bin/${SFML_OPENAL_ARCH}/openal32.dll
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/openal32.dll
    )
endif()
