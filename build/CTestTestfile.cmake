# CMake generated Testfile for 
# Source directory: /home/nod3zer0/Skola/Vysoka/5.semestr/ISA/LDAP-new
# Build directory: /home/nod3zer0/Skola/Vysoka/5.semestr/ISA/LDAP-new/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(berParserTestInt.BasicTests "/home/nod3zer0/Skola/Vysoka/5.semestr/ISA/LDAP-new/build/isa-ldapserver" "--gtest_filter=berParserTestInt.BasicTests")
set_tests_properties(berParserTestInt.BasicTests PROPERTIES  SKIP_REGULAR_EXPRESSION "\\[  SKIPPED \\]" _BACKTRACE_TRIPLES "/usr/share/cmake/Modules/GoogleTest.cmake;402;add_test;/home/nod3zer0/Skola/Vysoka/5.semestr/ISA/LDAP-new/CMakeLists.txt;37;gtest_add_tests;/home/nod3zer0/Skola/Vysoka/5.semestr/ISA/LDAP-new/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
