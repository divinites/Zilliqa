configure_file(${CMAKE_SOURCE_DIR}/constants.xml constants.xml COPYONLY)
link_directories(${CMAKE_BINARY_DIR}/lib)

# This test is no longer up-to-date after P2PComm has been changed to include broadcast_list_retriever
#add_executable (Test_P2PComm Test_P2PComm.cpp)
#target_include_directories (Test_P2PComm PUBLIC ${CMAKE_SOURCE_DIR}/src)
#target_link_libraries (Test_P2PComm PUBLIC Network Utils)

# This test is no longer up-to-date after P2PComm has been changed to include state
# (i.e., we can't run more than one Zilliqa instance now per process)
#add_executable (Test_PeerManager Test_PeerManager.cpp)
#target_include_directories (Test_PeerManager PUBLIC ${CMAKE_SOURCE_DIR}/src)
#target_link_libraries (Test_PeerManager PUBLIC Utils Network Zilliqa)

add_executable (Test_P2PComm Test_P2PComm.cpp)
target_include_directories (Test_P2PComm PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries (Test_P2PComm PUBLIC Network Utils)

add_executable (Test_IPFilter Test_IPFilter.cpp)
target_include_directories (Test_IPFilter PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries (Test_IPFilter PUBLIC Network Utils Boost::unit_test_framework)
add_test(NAME Test_IPFilter COMMAND Test_IPFilter)

add_executable (Test_Blacklist Test_Blacklist.cpp)
target_include_directories (Test_Blacklist PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries (Test_Blacklist PUBLIC Network Utils Boost::unit_test_framework)
add_test(NAME Test_Blacklist COMMAND Test_Blacklist)

add_executable (Test_ReputationManager Test_ReputationManager.cpp)
target_include_directories (Test_ReputationManager PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries (Test_ReputationManager PUBLIC Network Utils Boost::unit_test_framework)
add_test(NAME Test_ReputationManager COMMAND Test_ReputationManager)

add_executable (Test_Peer Test_Peer.cpp)
target_include_directories (Test_Peer PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries (Test_Peer PUBLIC Network)
add_test(NAME Test_Peer COMMAND Test_Peer)
