# toto getter
getting the results of soccer game from toto official page

## Various files
* toto_getter/src/accumulation.cpp

This is a C++ file that parse csv files. Please see the link below for the analysis.

### Build:
1. Installing boost
Windows: http://www.boost.org/doc/libs/1_63_0/more/getting_started/windows.html
UNIX: http://www.boost.org/doc/libs/1_60_0/more/getting_started/unix-variants.html

2. Building
```sh
$ cd toto_getter/src/
$ make
```

* toto_getter/src/csv/toto_get.py

This python script analyzes a Web page of toto and outputs it in csv file.

Installing Modules:
```sh
$ pip install beautifulsoup4 lxml progressbar2
```
## How to
At the first,Getting csv files.
```sh
$ cd toto_getter/src/csv
$ python3 toto_get.py
```
So looking infomation from csv data.
```sh
$ cd ..
$ ./accumulate
Data size: xxx
(?/p/s/d/q) >> 
```

## Man
* ?: Showing help
* p: Showing all loaded data.
* s: Outputting Game results of one team.
* d: Outputting Game results of two team.
* q: Quitting

## Another description
http://roki.hateblo.jp/entry/2016/12/19/toto%E3%81%AE%E5%85%AC%E5%BC%8F%E3%82%B5%E3%82%A4%E3%83%88%E3%81%8B%E3%82%89%E8%A9%A6%E5%90%88%E3%83%87%E3%83%BC%E3%82%BF%E3%81%A8%E3%81%8B%E3%82%92%E3%82%B9%E3%82%AF%E3%83%AC%E3%82%A4%E3%83%94%E3%83%B3
