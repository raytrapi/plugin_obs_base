
project(obs_bocina)

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/UI/obs-frontend-api")

file(GLOB CODIGO src/*.cpp)
#file(GLOB HPP "${CMAKE_SOURCE_DIR}/../../libobs/obs.hpp")
add_library(obs_bocina MODULE
      ${CODIGO}
      )
target_link_libraries(obs_bocina
      libobs obs-frontend-api)
 
install_obs_plugin_with_data(obs_bocina data)