cmake_minimum_required(VERSION 3.14)

project(
    Custom2DPlatformer
    VERSION 1.0
    DESCRIPTION "CS202 OOP Platformer Game"
    LANGUAGES CXX
)

# The application was moved into this directory. Keeping a small root entry
# point allows the usual `cmake -S . -B build` workflow to keep working.
add_subdirectory("Project-2DGame")
