cd /home/eyevievv/Dev/Personal/cpp/Projects/Mini_messenger
mkdir coverage
lcov --capture --directory . --output-file coverage/coverage.info
lcov --remove coverage/coverage.info '*/external/*' '*/tests/*' -o coverage/coverage_filtered.info
lcov --extract coverage/coverage_filtered.info '*/src/*' -o coverage/coverage_final.info
genhtml coverage/coverage_final.info --output-directory coverage