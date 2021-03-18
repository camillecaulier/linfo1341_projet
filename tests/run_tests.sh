# Note that this assumes to be called from the Makefile, you may want to adapt it.
echo "A very simple test"
./tests/test.sh
# Run the same test, but this time with valgrind #VALGRIND=1 ./tests/simple_test.sh
echo "A very simple test, with Valgrind"

#VALGRIND=1 ./tests/test.sh