# magick $1 -threshold 50% -compress none
magick $1 -threshold 50% -compress none ${1##*.}.pbm
