Build
=====

Build Peppa PEG
---------------

Assume you have cmake and gcc/clang installed.

.. code-block:: console

    $ cmake -E make_directory ./build
    $ cd build
    $ cmake ..
    $ make

More Cmake options include:

* `-DENABLE_UNISTR=On`: Enable building with libunistring.
* `-DENABLE_ADDRESS_SANITIZER=On`: Enable building with Clang AddressSanitizer.
* `-DENABLE_MEMORY_SANITIZER=On`: Enable building with Clang MemorySanitizer.
* `-DENABLE_CHECK=On`: Enable building and running checks.
* `-DENABLE_DOCS=On`: Enable building documentations.
* `-DENABLE_DEBUG=On`: Enable building with CC -g (debug) option.
* `-DENABLE_OPTIMIZATION=On`: Enable building with CC -O2.
* `-DENABLE_CLI=On`: Enable building command-line interface (utility peppa).

Development
------------

If you have a difficulty having a complete development environment for Peppa PEG, try Docker:

.. code-block:: console

    $ docker run --rm -v `pwd`:/app -it ubuntu:latest bash
    # apt-get install git gcc gdb valgrind make cmake libunistring-dev python3 python3-venv python3-pip doxygen
    # // you have all the dev-dependencies now.

Testing Peppa PEG requires downloading the test framework `Unity`:

.. code-block:: console

    # cd /app
    # git submodule init
    # git submodule update

If you have Valgrind installed, you can enable `ENABLE_VALGRIND`.

.. code-block:: console

    # cd build
    # cmake -DENABLE_VALGRIND=On ..

To build test,

.. code-block:: console

    # cmake -DENABLE_CHECK=On ..
    # cmake --build . --target check

To build docs,

.. code-block:: console

    # cmake -DENABLE_DOCS=On ..
    $ cmake --build . --target docs

GitHub Actions
--------------

Peppa PEG uses GitHub Actions building docs whenever a change is pushed to the main branch.

The docs site is pushed to gh-pages branch and published to <https://www.soasme.com/PeppaPEG/>.
