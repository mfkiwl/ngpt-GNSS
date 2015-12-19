# Introduction

This repository is an effort to construct:

* a (kind-of) elementary c++ gnss-related library, with focus on scientific applications
* a collection of basic programs for analyzing gnss (and gnss related) data

The development will be very-very slow ...

# Compilation
 
> :bangbang: Don't even try installing the project with a compiler not
> supporting at least c++11.

We are trying to keep the installation process compatible with
the [GNU Build System](https://en.wikipedia.org/wiki/GNU_build_system)
but this may not always be the case.

Clone the repository in a local folder:
```shell
$> git clone https://github.com/xanthospap/ngpt.git
```

(this will clone the repository into a local folder named `ngpt` at
the current path).

Go to the top directory of the project. The two files you will have
to customize (if needed) are `src/Makefile.am` and `test/Makefile.am`
But first, you have to run the `autoreconf` tool like (see [[1]](#autoreconf)):
```shell
$> autoreconf -i
```

If you do not need the development version (with a **lot** of
debuging info and no optimizations) then replace the two `Makefile.am`
files with their `Makefile.am.production` counterparts. That is:
```shell
$> cat src/Makefile.am.production > src/Makefile.am &&
   cat test/Makefile.am.production > test/Makefile.am
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

## Documentation

To compile the documentation (including the API reference), you need to have

1. [Doxygen](http://www.stack.nl/~dimitri/doxygen/)
2. [bibtex](http://www.bibtex.org/)

If available [graphviz](www.graphviz.org/) will be used to generate the documentation
graphs.

Go to the `/doc` folder and type:
```shell
$> doxygen doxy.conf
```

This will create the doc pages under `doc/html`; start at `index.html`.

## Tested Compilers & OSs

| Compiler    |  Version  |
|-------------|-----------|
| gcc-g++     | 5.3       |

# References

<a name="autoreconf">[[1]](https://www.gnu.org/savannah-checkouts/gnu/autoconf/manual/autoconf-2.69/html_node/autoreconf-Invocation.html)</a>
