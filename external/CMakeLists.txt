if(BUILD_TESTING)
  Include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.8.1
  )

  FetchContent_MakeAvailable(Catch2)
endif()

macro(find_package pkg)
  if(NOT TARGET "${pkg}")
    _find_package(${ARGV})
  endif()
endmacro()
