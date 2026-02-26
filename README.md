# pbmdots
This is about identifying the location (coordinates) of dots in an image.
It's done in an old-fashioned way, by first converting the png to pbm P1
using a threshold (50% is good).

# pixels to dot
So a key problem is that what looks like a dot is usually more than one pixel. Well if it was 3, fine
or any odd number in fact, that's not so bad, but often it is two pixels. Right so, which one to choose?
