# Compilation

Clone the repository in a local folder:
```shell
$> git clone https://github.com/xanthospap/ngpt.git
```

(this will clone the repository into a local folder named `ngpt` at
the current path).

Go to the top directory of the project. The two files you will have
to customize (if needed) are `src/Makefile.am` and `test/Makefile.am`
But first, you have to run the `autoreconf` tool like:
```shell
$> autoreconf -i
```

If you do not need the development version (with a **lot** of
debuging info and no optimizations) then replace the two `Makefile.am`
files with their `Makefile.am.production` counterparts. That is:
```shell
$> cat src/Makefile.a,.production > src/Makefile.am &&
   cat test/Makefile.a,.production > test/Makefile.am
```

If your compiler/compiler version does not support **c++14**, then
change that in the `Makefile.am` e.g.:
```shell
$> sed -i 's/-std=c++14/-std=c++11/g' src/Makefile.am
$> sed -i 's/-std=c++14/-std=c++11/g' test/Makefile.am
```

Go ahead and create the Makefiles; these are constructed based on the
`Makefile.am` you have set-up, using the configure script :
```shell
$> ./configure
```

Now you should be good to go! Just type:
````shell
$> make
```
to build the project.

You don't need to run `autoreconf` ever again! You need to run the
`configure` script though, if the `Makefile[.am]`'s change.

## Tested Compilers & OSs

| Compiler    |  Version  |
|-------------|-----------|
| gcc-g++     | 5.3       |
