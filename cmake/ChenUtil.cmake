# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen

# group files in the IDE(e.g: Xcode and Visual Studio)
function(chen_group_files prefix list)
    # check prefix length, include the trailing '/'
    string(LENGTH ${prefix} length)
    math(EXPR length "${length} + 1")

    foreach(name ${list})
        # retrieve the dirname
        get_filename_component(folder ${name} PATH)
        string(LENGTH ${folder} count)

        if(NOT (${length} GREATER ${count}))
            string(SUBSTRING ${folder} ${length} -1 group)
            string(REPLACE "/" "\\\\" group ${group})
        endif()

        # add files into group
        if(NOT group)
            source_group("" FILES ${name})
        else()
            source_group(${group} FILES ${name})
        endif()
    endforeach()
endfunction()

# generate definitions about build information
function(chen_build_system)
    find_program(perl "perl")
    find_program(lsb_release "lsb_release")

    if(APPLE AND perl)
        execute_process(
                COMMAND bash -c "system_profiler SPSoftwareDataType | perl -0 -pe 's/^.*System Version: ([^\n]+).*$/\\1/s'"
                OUTPUT_VARIABLE system
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        add_definitions(-DCHEN_BUILD_SYSTEM="${system}")
    elseif(UNIX AND lsb_release AND perl)
        execute_process(
                COMMAND bash -c "lsb_release -d -s | perl -0 -pe 's/^\"(.*)\"$/\\1/s'"
                OUTPUT_VARIABLE system
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        add_definitions(-DCHEN_BUILD_SYSTEM="${system}")
    else()
        add_definitions(-DCHEN_BUILD_SYSTEM="${CMAKE_SYSTEM}")
    endif()
endfunction()

function(chen_build_compiler)
    add_definitions(-DCHEN_BUILD_COMPILER="${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endfunction()

function(chen_build_timestamp)
    string(TIMESTAMP timestamp "%Y-%m-%d %H:%M:%S UTC" UTC)
    add_definitions(-DCHEN_BUILD_TIMESTAMP="${timestamp}")
endfunction()

function(chen_build_commit)
    execute_process(
            COMMAND git log -1 --format=%H
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE commit
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    add_definitions(-DCHEN_BUILD_COMMIT="${commit}")
endfunction()

function(chen_build_branch)
    execute_process(
            COMMAND git rev-parse --abbrev-ref HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE branch
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    add_definitions(-DCHEN_BUILD_BRANCH="${branch}")
endfunction()