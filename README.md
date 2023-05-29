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