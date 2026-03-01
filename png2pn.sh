# # note 50% is quite good at getting rid of slanting line which
# is quite grey. 75% will let alot of the slanting line through.
# magick $1 -threshold 50% -compress none
# magick $1 -threshold 50% -compress none ${1##*.}.pbm
magick $1 -threshold 40% -compress none ${1%.*}.pbm
