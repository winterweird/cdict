# CDict - hashtables(?) in C

This is a library providing functionality akin to a hashtable, except that you
can literally put in any key/value combinations you'd like in one single
hashtable, and as long as you set it up properly and don't fuck up it _should_
work.

## Installation

Clone this git repository:

```
git clone https://github.com/winterweird/cdict.git
```

Navigate to the cloned repository and run the following commands:

```
./configure
make
make install
```

(The last command may be ommitted if you don't actually want to install this
library. It works just as well if you manually copy it and the include files to
where you need to use it.)

## How to use

This library does not provide a _lot_ of functionality. It's pretty much just an
implementation of a hashtable (hashmap, associative array, dictionary, etc).
However, there are some things you can do! (And some things which, given that
you want to do some other things, you have to do.)

### Data structures

#### dict

A `dict` (short for "dictionary", in reminiscence of Python's dict datatype, due
to the fact that you can literally cram whatever you'd like into it) is simply a
structure keeping track of what it needs to know in order to function the way it
should. You don't need to know what's going on under the hood, you'll just be
interacting with it using the API anyways - well, that's the idea, but you can
do whatever. I don't judge.

But if you absolutely want to know, it's just an array of "buckets" (each bucket
is currently a linked list) and some variables which help it decide when that
array should grow. These variables are: The length of the array, how many of the
buckets are currently containing something, and some factor between 0 and 1
denoting how big a percentage of the buckets should contain something before the
array size doubles. The initial capacity and the factor can be set when you make
the dictionary, don't worry.

#### DictKVPair

A `DictKVPair` is a... dictionary key value pair. Each `DictKVPair` is basically
a node in a linked list, which is how collision resolution is handles in CDict!

That's... pretty much it. You don't really need to know how this one works
either.

#### DictItem

This is the structure you'll probably have to deal with directly the most. A
`DictItem` is the type of any key or value you can put in the dictionary. Each
`DictItem` object has a type and some data in it, and depending on the type, the
data will be interpreted and handled differently. Also, there are not a lot of
functions which let you operate on these structures, so you better learn how
they work or something like that. These structures are defined in
`dict_common.h`.

Each `DictItem` contains two variables: `type` and `item`. The `item` is itself
a union of a bunch of primitive data types and one "wildcard" data type - which
is basically a pointer to a structure of your choice, along with some useful
extra information which helps figuring out what to do with that structure. The
contents of the union are:

* `i`: A regular signed integer, or `int` for short. No, it's not a `short`,
  it's an `int`. Yes, I know I said... y'know, never mind.
* `l`: A regular signed long integer, or `long`. It's basically a normal
  integer, but there's MORE of it!
* `c`: A `char`. I have no idea why I felt the need to cram that one in there,
  but why not!
* `s`: A `char*`, or a "char pointer"," sometimes called a "string" by people
  who feel like "char pointer" has too many syllables. It might not be as
  primitive a data type as the numerical types, but it's so special and
  important that there's special support for it here too.
* `d`: A `double`, which is basically a real number but with errors. Be careful
  when adding them together.
* `o`: Stands for "object", which is basically short for "the rest".
  * `o.data`: A pointer to whatever structure
  * `o.str`: Contains a string representation - sorry, a _char pointer_
    representation - of the structure. Can be `NULL` if you don't give a shit.
  * `o.eq`: See, this is the sort of hackjob I do when I realize the first thing
    that popped into my mind won't work. This is a pointer to a function which
    takes two `DictItem`s and returns an `int`. The return value should be 1 if
    the items are to be considered equal, and otherwise 0. You won't have to
    call it directly, but you need to define it if you're gonna use custom data
    types as keys. And the function calling it has already checked that the two
    arguments are of the same type, so you can't cheat it. Don't even bother.
  * `o.size`: It says how many bytes it takes to store `o.data`, which is useful
    for memory reasons.

And the `type` is just a number. Number 0 through 5 are reserved, the rest are
free game. (But don't go over `MAX_INT`, whatever that is.)

Whew! That was a lot. Don't let it get to you, because there is more. Now we're
moving on to the API!

### API functions

#### `dict` functions

These are functions which are declared in `dict.h` and defined in `dict.c`. They
tend to deal with high-level operations on the dict, as well as some operations
which are not really operations on the dict, but which are grouped together in
these source files anyways. Notably, error functions, which are called when
something goes wrong so that you can get something useful instead of
"Segmentation fault (core dumped)".

* `dict dict_new(int initialCapacity, double loadFactor)`: It creates the `dict`
  with the initial conditions set as specified. I was inspired by Java's HashMap
  when I made this. The Java API documentation says 0.75 is a good value for
  `loadFactor`, so I'm gonna second that. The other option can be whatever you
  want it to be. Make it something reasonable, please.

* `void dict_put(dict* m, DictItem k, DictItem v)`: You put a key/value pair
  with key `k` and value `v` in the dictionary. It's pretty straightforward.
  Note that the dictionary argument must be a pointer, because otherwise there
  are memory problems.

* `DictItem dict_get(dict m, DictItem k)`: You retrieve the value of the pair
  with the given key. Also pretty straightforward, although the tricky part can
  be extracting whatever you want from that value DictItem. Note that the
  dictionary passed here is NOT a pointer, because I'm a moron who cannot keep
  his API consistent. My best excuse is that this clearly distinguishes parts of
  the API which change the dictionary with parts which just query it. But if I'm
  honest, I just threw something together and added the pointer parts when I
  realized why "something" didn't work, then just didn't bother to update the
  rest of the API. Oh, and if the key does not exists, it returns a nulltype
  `DictItem`. That's important. You can use it to find out which keys exist and
  stuff.

* `DictItem dict_get_strict(dict m, DictItem k)`: I realized the need for this
  after spending 15 minutes trying to track down the bug which resulted in a
  gibberish return value before realizing that I had sent the dictionary a key
  which didn't exist in it. Basically, you use this when you KNOW the key exists
  in there and you just want to get it. The advantage to this approach is that
  it just outright fails if the key doesn't exist, so it's a lot easier to
  figure out that you've fucked up on something stupid.

* `void dict_free(dict m)`: This hopefully frees all the allocated resources,
  unless I've fucked up somewhere. Also, if I were consistent with the excuse I
  made earlier, this would receive a pointer argument, but I _think_ a
  non-pointer is sufficient...

##### error functions

All of these functions print a message to stderr and exit with a nonzero exit
code. (The user of the library might not need these.)

* `void dict_error_memerror(unsigned long sz)`: Called if a memory allocation
  failed, argument is the size of the attempted allocation. I tried to
  consistently use this for all allocations, but I _might_ have given up
  halfway. Not sure.

* `void dict_error_message(const char* msg)`: Most generic error function of
  them all: just prints "Error: " and the message, then exits.

* `void dict_error_itemmessage(const char* msg, DictItem i)`: Prints the message
  followed by a space followed by a string representation of the item, then
  exits. For user defined data types, the string representation is the contents
  of `item.o.str`.

* `void dict_error_apierror(const char* msg)`: I added this in order to have a
  clear-cut separation between this and plausible user errors. Basically, these
  are sanity checks - error messages which _should never happen_. If they do,
  that means there's a problem with the API which needs to be fixed, not a
  problem happening because I accidentally misspelled something or whatever.

#### `dict_hash` functions

Or, uh, function. It's just one. But it's an important one!

* `int dict_item_hash(DictItem i)`: You shouldn't need to call this one
  yourself, but it's basically what gives a hashtable its fast lookup times. A
  good hash function is able to disperse different data in different buckets so
  that the number of elements in each bucket is low. That means you can abandon
  a linear search in favor of a constant-time direct lookup - in theory. In
  practice, you sometimes need to search a little bit, but not much, so it's
  close to O(1) in complexity. Which is sorta cool, I guess.

  I didn't write the hashing function, btw. Scroll down to the bottom of the
  README for credit where it's due.

#### `dict_item` functions

These are arguably the second most important group of functions, right after the
actual `dict_put` and `dict_get` functions. You use them to operate on `DictItem`
objects, which you'll remember, if your attention span is above that of half a
goldfish, are the stuff keys and values are made of, so they're kinda important.

##### item creation functions

* `DictItem dict_item_new_null()`: Returns a nulltype dict item. Normally you'll
  be more concerned about receiving one of those than about making one yourself.
* `DictItem dict_item_new_int(int v)`: Creates a `DictItem` of type `int`.
* `DictItem dict_item_new_long(long v)`: Creates a `DictItem` of type `long`.
* `DictItem dict_item_new_char(char v)`: Creates a `DictItem` of type `char`.
  (Why did I decide to have a `char` type item, again?)
* `DictItem dict_item_new_string(char* v)`: Creates a `DictItem` of type, uh,
  _char pointer_.
* `DictItem dict_item_new_double(double v)`: Creates a `DictItem` ~~on the
  double~~ of type `double`.
* `DictItem dict_item_new_object(int type, void* ptr, unsigned int sz, dict_eq_t
  eq)`: This basically just sets each field in the `DictItem` directly to the
  value it's supposed to be. You must allocate the pointer `ptr` yourself. Don't
  forget to add the `eq` function. Also, you should probably be consistent with
  which type you use for which kind of data structure. Consistency is useful.
  (Note that there is no argument that sets the `str` part of the object - it's
  set to NULL in the function, so if you want it to be something else you'll
  have to set it afterwards.) Typically, this is a function you'll call in the
  custom item creation function you've devised yourself, although you might just
  be a masochist and decide to manually set up every key/value pair you need...

##### item manipulation functions

* `int dict_item_null(DictItem)`: Yep, just checks if the item is nulltype.
  Return value is 1 if the item is nulltype, else 0. Nothing to see here...
* `int dict_item_equals(DictItem a, DictItem b)`: Checks if two items are to be
  considered equal. Two items should be considered equal if:
  * Their types are the same, and
    * They are primitive types and equal as per built-in equality checks, or
    * They are user-defined types and equal as per the equality function given
      to `DictItem a`.
  
  Note that this means it's potentially possible to do a great deal of fuckery
  by giving different items of the same type different equality functions. My
  simple advice to the people who just thought of how they might break this:
  Don't. This tool wasn't invented to make it difficult to shoot oneself in the
  foot. I'd say it's slightly easier to shoot oneself in the foot than to shoot
  anything else, given that "anything else" is a user-defined target. But either
  one is possible, so if you don't want to end up on a stretcher... don't miss.
* `void dict_item_free(DictItem i)`: Frees allocated resources of the item, if
  applicable. What this basically means is that it frees the _char pointer_
  allocated in `dict_item_new_string` and the pointer you allocated in your
  custom item creation function, along with any `str` allocation you created if
  you created any `str` allocations. Actually, it tries to free `str` anyways,
  so if it's not set to NULL, you better make sure you _do_ allocate the memory
  because if not then you'll probably get a segmentation fault, and we all know
  how pleasant those are.

## THE END

If you've made it through to the very end, you might wonder how a crazy dude
like me ever managed to write code coherent enough to even compile. The fact of
the matter is that it's 8 AM in the morning as of the time of writing, and I'm
writing this README after spending the better part of the night trying to figure
this stuff out, so right now I'm basically letting the "creative writing" part of
my brain take over while the "think like a computer" part rests. If you've made
it all the way to the end and you still don't understand the part of the API you
set out to figure out, you may look at the comments in the header files, which
are a bit more sane and helpful, or if everything else fails then - god forbid -
the code itself. Just hope you'll never get that desperate.

I'll probably regret writing the README like this later on when my brain has
started processing stuff again, but meh. Also, my deepest apologies for wasting
your time with my rambling. I hope you at least found it occasionally mildly
amusing.

## Credits

The function used to compute the hashes is the SuperFastHash function by Paul
Hsieh, which can be found [here](http://www.azillionmonkeys.com/qed/hash.html).
It's licensed under the [LGPL 2.1 license](https://www.gnu.org/licenses/lgpl-2.1.txt),
which I started reading, and got through maybe around one third of it
(essentially the preamble) before giving up and deciding it's probably fine if I
include a [link](https://www.gnu.org/licenses/lgpl-2.1.txt) to
[the license](https://www.gnu.org/licenses/lgpl-2.1.txt) somewhere in
[this document](https://www.gnu.org/licenses/lgpl-2.1.txt). Don't sue me.

Many of the design decision I made throughout this project were inspired by the
[Java API documentation for
HashMap](https://docs.oracle.com/javase/7/docs/api/java/util/HashMap.html).
However, many of the more _questionable_ decisions were my own, so if you see
something in the code which doesn't quite make sense, that was probably my
fault. Oops.

I think that's about it. I'll possibly be updating this project once in a while.
Until then, this is what I've got. Bye bye!
