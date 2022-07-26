![GitHub release (latest by date)](https://img.shields.io/github/v/release/Sound-Linux-More/libopustags)
![GitHub Release Date](https://img.shields.io/github/release-date/Sound-Linux-More/libopustags)
![GitHub repo size](https://img.shields.io/github/repo-size/Sound-Linux-More/libopustags)
![GitHub all releases](https://img.shields.io/github/downloads/Sound-Linux-More/libopustags/total)
![GitHub](https://img.shields.io/github/license/Sound-Linux-More/libopustags)

opustags [fork](https://github.com/fmang/opustags)
========

Library and tool for view and edit Opus comments.

It was built because at the time nothing supported Opus, but now things have
changed for the better.

For alternative, check out these projects:

- [EasyTAG](https://wiki.gnome.org/Apps/EasyTAG)
- [Beets](http://beets.io/)
- [Picard](https://picard.musicbrainz.org/)
- [puddletag](http://docs.puddletag.net/)
- [Quod Libet](https://quodlibet.readthedocs.io/en/latest/)
- [Goggles Music Manager](https://gogglesmm.github.io/)

See also these libraries if you need a lower-level access:

- [TagLib](http://taglib.org/)
- [mutagen](https://mutagen.readthedocs.io/en/latest/)

Requirements
------------

* A POSIX-compliant system,
* libogg.

Installing
----------

    ./autogen.sh
    ./configure --prefix=/usr/local
    make
    sudo make install

Documentation
-------------

    Usage: opustags --help
           opustags [OPTIONS] FILE
           opustags OPTIONS FILE -o FILE

    Options:
      -h, --help              print this help
      -o, --output FILE       write the modified tags to a file
      -y, --overwrite         overwrite the output file if it already exists
      -v, --verbose           verbose (debug) mode (for corrupt file and development)
      -d, --delete FIELD      delete all the fields of a specified type
      -a, --add FIELD=VALUE   add a field
      -s, --set FIELD=VALUE   delete then add a field
      -p, --picture FILE      add cover
      -D, --delete-all        delete all the fields!
      -S, --set-all           read the fields from stdin

See the man page, `opustags.1`, for extensive documentation.
