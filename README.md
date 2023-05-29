# Algorithmic Information Theory - 2nd Project

Development Team:
| Nmec  | Name            | Email                    |
|-------|----------------|--------------------------|
| 97484 | Pedro Monteiro | pmapm@ua.pt              |
| 98512 | Eduardo Fernandes | eduardofernandes@ua.pt |
| 98597 | José Trigo        | josetrigo@ua.pt          |

# Repository Structure
- bin
    - compiled program files
- examples
    - text files used for tests
- langs
    - text files in different languages from https://sourceforge.net/projects/la-strings/files/Language-Data/
- report
    - report and documentation of assignment
- src
    - files related to the developed code (lang, findlang and locatelang)
## How to run lang program
Change to src folder
```
cd src/
```

Compile
```
g++ -std=c++11 lang.cpp -o lang
```

Just run the executable file with the parameters
```
./lang <reference_filename> <target_filename> <k> <alpha> <fail_threshold>
```

## How to run findlang program
Change to src folder
```
cd src/
```

Compile
```
g++ -std=c++11 findlang.cpp -o findlang
```

Just run the executable file with the parameters
```
./findlang <reference_files_directory> <target_filename> <k> <alpha> <fail_threshold>
```

## Example for lang
Run the lang program with the portuguese text in langs folder (pt_PT.Portugese-latn-EP7.utf8) as reference and  the maias.txt file in examples folder as target text. First it is necessary to change to src/ folder
```
cd src/
```

After that, you should compile the lang program code
```
g++ -std=c++11 lang.cpp -o lang
```

Finally, just run the executable with the reference and target filenames, k, alpha and threshold. In this case was used a k=3, alpha=1 and threshold=3
```
./lang ../langs/pt_PT.Portugese-latn-EP7.utf8 ../examples/maias.txt 3 1 3
```

This should be printed in the terminal
```
Hits: 389255
Fails: 812099
Estimated total bits: 2.72992e+06
Average bits per symbol: 2.08557
```

## Example for findlang
First it is necessary to change to src/ folder
```
cd src/
```

After that, you should compile the findlang program code
```
g++ -std=c++11 findlang.cpp -o findlang
```

Finally, just run the executable passing the langs folder (wich contain several texts in distinct languages), the target text(text to be analyzed), k (sequence size), alpha and the fails threshold.. In this case was used a text in portugues (national anthem), a k=3, alpha=1 and threshold=3
```
./findlang ../langs/ ../examples/hino.txt 3 1 3
```

This should be printed in the terminal
```
Estimated total bits for sr_SR.Serbian-latn-wiki.utf8: 1286.21
Estimated total bits for he_IL.Hebrew-wiki.utf8: 1886.53
Estimated total bits for nds_NL.Dutch_Low_Saxon-wiki.utf8: 1089.5
Estimated total bits for ro_RO.Romanian-latn-EP7.utf8: 1162.85
Estimated total bits for it_IT.Italian-latn-EP7.utf8: 1187.38
Estimated total bits for arz_EG.Egyptian_Arabic-wiki.utf8: 1821.72
Estimated total bits for tr_TR.Turkish-latn-wiki.utf8: 1165.69
Estimated total bits for es_ES.Spanish-latn-EP7.utf8: 1041.31
Estimated total bits for en_GB.English-latn-EP7.utf8: 1249.04
Estimated total bits for hr_HR.Croatian-wiki.utf8: 1123.95
Estimated total bits for el_GR.Greek-grek-EP7.utf8: 1598.15
Estimated total bits for sv_SE.Swedish-latn-EP7.utf8: 1234.66
Estimated total bits for ru_RU.Russian-wiki.utf8: 1637.97
Estimated total bits for pt_PT.Portugese-latn-EP7.utf8: 776.23
Estimated total bits for haw_US.Hawaiian-wiki-test.utf8: 1675.9
Estimated total bits for de_DE.German-latn-EP7.utf8: 1332.05
Estimated total bits for uk_UA.Ukrainian-wiki.utf8: 1684.58
Estimated total bits for ar_XX.Arabic-wiki.utf8: 2272.36
Estimated total bits for fr_FR.French-latn-EP7.utf8: 1096.18
Estimated total bits for ja_JP.Japanese-wiki.utf8: 2093.43
The text is likely to be written in pt_PT.Portugese-latn-EP7.utf8: 776.23
```