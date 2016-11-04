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