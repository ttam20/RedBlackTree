enable_testing()

configure_file(rbtree.json ${CMAKE_BINARY_DIR}/tests/rbtree.json COPYONLY)
configure_file(rbtree.txt ${CMAKE_BINARY_DIR}/tests/rbtree.txt COPYONLY)
configure_file(rbtree.xml ${CMAKE_BINARY_DIR}/tests/rbtree.xml COPYONLY)

add_executable(TestRedBlackTree testredblacktree.cpp)
add_test(NAME TestRedBlackTree COMMAND TestRedBlackTree)
target_link_libraries(TestRedBlackTree PRIVATE Qt${QT_VERSION_MAJOR}::Test RedBlackTreeLib)

