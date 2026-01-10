
if(NOT EXISTS "/home/jared/new-code/MasterX_System/cmd/build/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: /home/jared/new-code/MasterX_System/cmd/build/install_manifest.txt")
endif()

file(READ "/home/jared/new-code/MasterX_System/cmd/build/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")

foreach(file ${files})
  message(STATUS "Uninstalling: ${file}")
  if(EXISTS "${file}")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${file}"
      COMMAND ${CMAKE_COMMAND} -E remove "${file}"
      OUTPUT_QUIET
      ERROR_QUIET
    )
    # Check if it was a directory and if it's now empty, try to remove it
    # This is a simplified approach; more robust directory removal might be needed
    if(IS_DIRECTORY "${file}")
        # This part is tricky as 'remove_directory' might fail if not empty due to other files
        # A more robust solution would involve listing contents and removing them first
        # or using platform-specific commands.
        # For now, we rely on the fact that install_manifest.txt lists files individually.
        # If a directory is listed, it's usually because it was created by 'install(DIRECTORY...)'.
    endif()
  else()
    message(STATUS "File not found: ${file}")
  endif()
endforeach()
message(STATUS "Uninstallation complete.")
