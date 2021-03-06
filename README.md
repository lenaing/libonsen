libonsen
========

Intro
-----

This library offers an easy to use framework to open and manipulate (mostly
games) data archives through an extensive plugins usage.

As such, it can't be used alone. This library should be linked to an executable
which will take benefit from the library functions.

Features
--------

### Plugin API

With `libonsen`, you can manipulate `libonsen` plugins easily through the plugin
API. This API is detailed in the _doc/API_ document. Please read this file
thorougfully so you can create and use `libonsen` plugins!

#### Archive Plugins

Archive plugins are plugins designed to open data archives, pretty much like
_*zip_ utilities.

Install
--------

### From sources

#### Getting the sources

##### Git

    $ git clone git@github.com:lenaing/libonsen.git

#### Compile

As a normal user :

    $ cd libonsen
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

#### Install

As superuser :

    # make install

Homepage
--------

    TODO

Bug reports to
--------------

    Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>

Copyright
---------

`libonsen` is released under a
[CeCILL-C](http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html)
free software license. Please see COPYING and LICENSE (en) or LICENCE (fr) files
for more information on the license.
