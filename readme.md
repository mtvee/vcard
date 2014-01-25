vcard
-----

A simple query utility for an address book saved in vcard format. I use this
from `mutt` in lieu of [abook][1] and [rolo][2].

Requirments
-----------

- *nix environment
- c++11

Install
-------

run `make` and put the binary `vcard` somewhere on your path. The binary looks
for its data file in `$HOME/.vcard/contacts.vcf`. I just export my google
contacts to this file every once in a while.

Usage
-----

    vcard --help

Mutt
----

You can add the following to your `muttrc`

    set query_command = "vcard '%s'"
    bind editor <Tab> complete-query

Licence
-------

vcard is licensed under the [GPL3][3]

TODO
----

- be able to add/edit/delete records
- folded lines aren't handled properly


[1]: http://abook.sourceforge.net/
[2]: http://rolo.sourceforge.net/
[3]: http://www.gnu.org/licenses/gpl-3.0.txt
