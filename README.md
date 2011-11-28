libonsen
========

Intro
-----

This library offers an easy to use framework to open and manipulate (mostly
games) data archives through an extensive plugins usage.

As such, it can't be used alone. This library should be linked to an executable
which will take benefit from the library functions.

Functionnalities
----------------

### Plugin API

With `libonsen`, you can manipulate `libonsen` plugins easily through the plugin
API. This API is detailed in the _doc/API_ document. Please read this file
thorougfully so you can create and use `libonsen` plugins!

#### Archive Plugins

Archive plugins are plugins designed to open data archives, pretty much like
_*zip_ utilities.

### Iconv Helper

The [libiconv](http://www.gnu.org/s/libiconv/) library enable an application to
support multiple character encodings.
The `libonsen` library include a wrapper to take care of `libiconv` calls.
The `libiconv`, `libcharset` libraries and their header files are under
[LGPL v2.1](http://www.gnu.org/licenses/lgpl-2.1.html).

### Shift-JIS Helper

Thanks to the Iconv support, you can manipulate Shift-JIS strings and translate
them in another encoding. Please note that currently only Shift-JIS to UTF-8
conversion is supported.

Copyright
---------

`libonsen` is released under a
[CeCILL-C](http://www.cecill.info/licences/Licence_CeCILL-C_V1-en.html)
free software license. Please see COPYING and LICENSE (en) or LICENCE (fr) files
for more information on the license.

Download
--------

TODO

Homepage
--------

    https://tracker.ziirish.info/projects/libonsen/

Bug reports to
--------------

    Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
