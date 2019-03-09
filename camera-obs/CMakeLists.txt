cmake_minimum_required(VERSION 2.8)

project(camera-obs)

set(ROS_DIR /opt/ros/kinetic/)
#set(ROS_DIR /opt/ros/melodic/)
set(DDS_ROOT_DIR /software/OpenDDS-3.13)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include

    ${DDS_ROOT_DIR}
    ${DDS_ROOT_DIR}/ACE_wrappers
    ${DDS_ROOT_DIR}/ACE_wrappers/TAO
    ${PROJECT_SOURCE_DIR}/include/opendds
    ${PROJECT_SOURCE_DIR}/include/opendds/idl

    ${ROS_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/ros
    )

link_directories(
    ${ROS_DIR}/lib
    /usr/local/lib
    )

link_libraries(
    roscpp
    rosconsole
    roscpp_serialization
    rostime
    )


file (GLOB_RECURSE OPENDDS_SRC ${PROJECT_SOURCE_DIR}/src/opendds/*.cpp)
file (GLOB_RECURSE ROS_SRC ${PROJECT_SOURCE_DIR}/src/ros/*.cpp)

file (GLOB_RECURSE IDL_MSGS_SRC ${PROJECT_SOURCE_DIR}/include/opendds/idl/*.cpp)

add_library(camera-obs SHARED ${OPENDDS_SRC} ${IDL_MSGS_SRC})  

add_executable(obs_dds_ros "obs_dds_ros.cpp" ${OPENDDS_SRC} ${IDL_MSGS_SRC} ${ROS_SRC})

add_executable(obs_dds_pub "obs_dds_pub.cpp" ${OPENDDS_SRC} ${IDL_MSGS_SRC})


file (GLOB OpenDDS_LIBS "${DDS_ROOT_DIR}/lib/*.so")
foreach (i ${OpenDDS_LIBS})
    get_filename_component (BASE_NAME_DLL ${i} NAME_WE)
    add_library (${BASE_NAME_DLL} SHARED IMPORTED)
    set_property (TARGET ${BASE_NAME_DLL} PROPERTY IMPORTED_LOCATION "${DDS_ROOT_DIR}/lib/${BASE_NAME_DLL}.so")
    target_link_libraries (obs_dds_ros ${BASE_NAME_DLL})
    target_link_libraries (obs_dds_pub ${BASE_NAME_DLL})
endforeach()

file (GLOB OpenDDS_LIBS "${DDS_ROOT_DIR}/ACE_wrappers/lib/*.so")
foreach (i ${OpenDDS_LIBS})
    get_filename_component (BASE_NAME_DLL ${i} NAME_WE)
    add_library (${BASE_NAME_DLL} SHARED IMPORTED)
    set_property (TARGET ${BASE_NAME_DLL} PROPERTY IMPORTED_LOCATION "${DDS_ROOT_DIR}/ACE_wrappers/lib/${BASE_NAME_DLL}.so")
    target_link_libraries (obs_dds_ros ${BASE_NAME_DLL})
    target_link_libraries (obs_dds_pub ${BASE_NAME_DLL})
endforeach ()



