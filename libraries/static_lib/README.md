*g++ -c generator.cpp -o generator.o* <!-- -c (compile) -->
<!-- generator.o -> machine instr -->

*nm generator.o*
<!-- 0000000000000000 T _Z5hellov
                      U puts      -->

*ar rcs libgenerator.a generator.o*
<!-- ar (archive) r (paste/replace) c (create without warning) s (add symbols (for linker)) -->

*ar t libgenerator.a*
<!-- 'generator.o' -->

| switch | phase | error, something missing |
|:------:|:-----:|:------------------------:|
| -I cesta | compile | fatal error: generator.h: No such file |
| -L cesta | linking | cannot find -lgenerator |
| -lgenerator |	linking | undefined reference to 'generate_random_numer()' |

g++ main.cpp -I../static_lib -L../static_lib -lgenerator -o generate