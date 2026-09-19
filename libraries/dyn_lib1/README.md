*g++ -fPIC -c compute.cpp -o compute.o*
<!-- *-fPIC = Position Independent Code* -->

*g++ -shared -o libcompute.so compute.o*

*g++ main2.cpp -I../dyn_lib1 -L../dyn_lib1 -lcompute -o compute*

*ldd compute*

*export LD_LIBRARY_PATH=../dyn_lib1*
<!-- ./compute -->

*export LD_LIBRARY_PATH=../dyn_lib2*
<!-- ./compute -->

*nm -D ../dyn_lib1/libcompute.so | grep compute*
*nm -D ../dyn_lib2/libcompute.so | grep compute*