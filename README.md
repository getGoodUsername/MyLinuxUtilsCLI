# MyLinuxUtilsCLI
> My command line utilities repo for a unix like system. Most of them are just thin wrappers on established utilities that I would like to use often, but don't want to memorize usage patterns.
> The utilities all start with "og" to avoid conflicts between other binaries available on the system path, and are my initials. Everything after "og" is the "real" name of the command
## `ogcopy`
- pipe in output from a command to your clipboard! must have `xclip` installed
- ##### Options
	- -s|--show
		- Acts like `tee` and prints to stdout whatever was redirected to `ogcopy` in addition to copying the input to your clipboard. Useful to inspect immediately output or to use within a pipeline of commands.
- ##### Usage
	- `ogcopy` \[-s]
	- `ogpass | ogcopy -s`
		- generate a password and copy to clipboard in addition to printing password to stdout
	- `ogcopy < file.txt`
		- copy the contents of `file.txt` to your clipboard
- ##### Things to Note
	- Since it is dependent on X windows, ssh connections that don't forward X11 won't work as expected to copy to 'clipboard'
		- Have to default to good old pipeling and redirecting or use a tool like tmux to copy the contents of the screen.
	- Only works with X windowing systems. Sorry wayland.

## `ogpass`
- generate a random password quickly
- ##### Usage
	- `ogpass` \[password length int] \[set of unallowed chars]
	- `ogpass`
		- prints a password 14 to 28 characters long that can have any printable character other than space
	- `ogpass 50`
		- prints a password 50 characters long made up of any printable character other than space
	- `ogpass 42 '%&*'`
		- prints a password 42 characters long made up of any printable character other than space, %, &, and \*
	- `ogpass 20 'a-z'`
		- prints a password 20 characters long made up of any printable character other than space, and a through z lowercase.
- ##### Things to Note
	- Default length: 14 to 28 chars inclusive
	- Default allowed charset: `[:graph:]` aka all printable chars other than space
	- `ogpass` will NEVER print a 'space' character
	- `ogpass` will ALWAYS output 1 byte characters
	- remember, the set of unallowed chars goes directly into `tr` so you can use that set in creative ways! Read `man tr`

## `ogsshkey`
- generate a safe ssh key quickly
- ##### Usage
	- `ogsshkey` \<base name of new key pair> \[comment in keys] \[KDF rounds] \[location of new keys]
	- `ogsshkey new-key`
		- Creates a key pair `new-key.pub` and `new-key` and puts them in ~/.ssh where the comment is `new-key` (the same as the base name of the key pairs), and KDF rounds = 100
	- `ogsshkey hello_world 'this is an ssh key for a cool computer' 250 "$PWD"`
		- Creates a key pair `hello_world.pub` and `hello_world` and put them in the current dir where the comment is `this is an ssh key for a cool computer` and KDF rounds = 250
- ##### Things to Note
	- base name of the new key pair is REQUIRED
	- Default comment is the same as the base name of the new key pair
	- Default KDF rounds = 100
	- Default output key location = ~/.ssh
	- Will make a new key that uses the `ed25519` algo

## `ograndwords`
- generate a collection of random words, one word per line
- ##### Usage
	- `ograndwords` \[word count]
	- `ograndwords`
		- prints 1 random word
	- `ograndwords 15`
		- prints 15 random words each on their own new line
- ##### Things to Note
	- if requested amount of words is greater than the number of unique words available in hard coded *word file*, the output will have repeat words
	- is dependent on a hard coded *word file*
		- set to `/usr/share/dict/words`

## `ogphrase`
- generate a pass phrase quickly
- ##### Usage
	- `ogphrase` \[word count] \[word separator]
	- `ogphrase`
		- prints 2 to 6 words separated by '-'
	- `ogphrase 5`
		- prints 5 words separated by '-'
	- `ogphrase 10 ';'`
		- prints 10 words separated by ';'
- ##### Things to Note
	- output is always in lowercase
	- any single word in the phrase will never end with apostrophe s (*'s*)
	- is dependent on a hard coded *word file*
		- set to `/usr/share/dict/words`

## `ogspot`
- `librespot` spotify headless player default settings
- not much more than that, just wrapped into a bash script to make easy to edit and run. I don't feel like remembering all sorts of options
- ##### Things to Note
	- is dependent on [`librespot`](https://github.com/librespot-org/librespot)
	- in order to get auth, launch script, open spotify client on phone and switch output to the new device. Auth will be received and script can now just run normally in the future.

## `ogpaste`
- `xclip` wrapper, just use default options for pasting
- not much more than that, just wrapped into a bash script to make easy to edit and run. I don't feel like remembering all sorts of options
- ##### Things to Note
	- is dependent on `xclip`
	- last new line will always be removed
	- the output will always be piped to stdout

## ***WIP***`ogbright`
- changes the brightness of most desktop monitors
- ##### Usage
	- `ogbright` \<brightness value, 0 - 100>
	- `ogbright 20`
		- sets all connected monitors to a brightness value of 20
- ##### Things to Note
	- is dependent on [`ddcutil`](https://www.ddcutil.com/)
	- brightness value is a REQUIRED argument.
	- No error checking currently set, please don't try to put negative numbers or numbers greater than 100 for brightness... just in case
# Deprecated
- `ogtracker`
	- A cmdline task tracker
	- Made for fun, better tools elsewhere
- `ogdim`
	- Dim laptop screen
	- Made way too complicated, should have only made `ogbright`
- `ogtimer`
	- Just a timer, counting down from an integer value minutes
	- Made for fun, better tools elsewhere
