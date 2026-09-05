if (NOT EXISTS "C:/dev/cpp/helloworld/build/vcpkg_installed/vcpkg/blds/sdl3/x64-windows-rel/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"C:/dev/cpp/helloworld/build/vcpkg_installed/vcpkg/blds/sdl3/x64-windows-rel/install_manifest.txt\"")
endif()

file(READ "C:/dev/cpp/helloworld/build/vcpkg_installed/vcpkg/blds/sdl3/x64-windows-rel/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
    message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
    execute_process(
        COMMAND C:/Users/lenan/AppData/Local/vcpkg/downloads/tools/cmake-4.3.2-windows/cmake-4.3.2-windows-x86_64/bin/cmake.exe -E remove "$ENV{DESTDIR}${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
    )
    if(NOT ${rm_retval} EQUAL 0)
        message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif (NOT ${rm_retval} EQUAL 0)
endforeach()
