#!/bin/sh

##
#  Create a directory listing of HTML files, in HTML format.
#  Used to autogenerate the list of benchmark/tune plots for Doxygen.
#

echo "<ul>"
for file in `ls $1/*.html`; do
    href=`basename ${file}`
    name=`basename ${file} .html | sed -e "s/^[a-z]\+-//g"`
    echo "<li><a class=\"el\" href=\"${href}\">${name}</a></li>"
done
echo "</ul>"
