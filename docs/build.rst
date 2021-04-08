Build
=====

Build Peppa PEG
---------------

Assume you have cmake and gcc installed.

.. code-block:: console

    $ cmake -E make_directory ./build
    $ cd build
    $ cmake ..
    $ make

Development
------------

If you have a difficulty having a complete development environment for Peppa PEG, try Docker:

.. code-block:: console

    $ docker run --rm -v `pwd`:/app -it ubuntu:latest bash
    # apt-get install git gcc gdb valgrind make cmake python3 python3-venv python3-pip doxygen
    # // you have all the dev-dependencies now.

Testing Peppa PEG requires downloading the test framework `Unity`:

.. code-block:: console

    $ git submodule init
    $ git submodule update

If you have Valgrind installed, you can enable `ENABLE_VALGRIND`.

.. code-block:: console

    $ cmake -DENABLE_VALGRIND=On ..

If you have doxygen and Python3 installed, you can enable `ENABLE_DOCS`.

.. code-block:: console

    $ cmake -DENABLE_DOCS=On ..

To build test,

.. code-block:: console

    $ cmake --build . --target check

To build docs,

.. code-block:: console

    $ cmake --build . --target docs

GitHub Actions
--------------

Peppa PEG uses GitHub Actions building docs whenever a change is pushed to the main branch.

The docs site is pushed to gh-pages branch and published to <https://www.soasme.com/PeppaPEG/>.
