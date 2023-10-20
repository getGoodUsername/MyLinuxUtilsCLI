# MyLinuxUtilsCLI
My command line utilities for a unix like system.
"og" are my initials, everything past that is the "real" name of the command

## ogcopy
pipe in output from a command to your clipboard! must have "xclip" installed

## ograndwords
need random english words? use this. you can either get one word with no arguments or "n" words with "ograndwords n"

## ogpasswordgen
quickly make a random password of 'n' characters and optionally also can send a second argument to specify any other special characters to be used other than the standard alphanumeric characters ('^%#$#!@', etc.)

## ogpassphrasegen
just like ogpasswordgen except now you are making a passphrase. The first
arguement decides how many words in the phrase you would like and the second
argument determines the word seperator in the phrase.

## ogsshkeygen
create a quick ssh key with all my prefered options. Give the keys a name with the first args,
ex. 'schoolServer', 'github', 'work', etc. And dictate where the key goes to. Default is ~/.ssh

## ogtracker
track my time by quickly "punching in and out" with a quick name of the current task
and a small decscription. A git commit for my life! Remember to have "OG\_TRACKER\_DIR" enviormenet variable that points to a directory
