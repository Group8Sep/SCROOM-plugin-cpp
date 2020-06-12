function(enable_doxygen)
  option(ENABLE_DOXYGEN "Enable doxygen doc builds of source" OFF)
  if(ENABLE_DOXYGEN)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_PROJECT_NUMBER ${GIT_PROJECT_VERSION})
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_EXTRACT_ANON_NSPACES YES)
    set(DOXYGEN_INLINE_SOURCES YES)
    set(DOXYGEN_REFERENCED_BY_RELATION YES)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_UML_LOOK YES)
    set(DOXYGEN_UML_LIMIT_NUM_FIELDS 5)
    set(DOXYGEN_TAB_SIZE 2)

    find_package(Doxygen REQUIRED dot)
    doxygen_add_docs(doxygen-docs ${PROJECT_SOURCE_DIR})

  endif()
endfunction()
