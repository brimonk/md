# markdown

Honestly, I wanted my own markdown processor so I could implement my own extensions.

It's purposefully simple and straight forward to allow for more and more functions and features.

### Building

```sh
make
```

### Running

If files are available in `$1` and `$2`, markdown will use those instead of `stdin` and `stdout`.

```
USAGE:
    ./md <[input]> <[output]>
```

