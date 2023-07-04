[![CI](https://github.com/vladimir-popov/laby/actions/workflows/ci.yaml/badge.svg)](https://github.com/vladimir-popov/laby/actions/workflows/ci.yaml)

![logo](logo.png)

# About:
<pre>
NAME
        labyrinth - a game about looking for exit from the labyrinth.
OPTIONS
        -s      an initial seed of the game. Used to generate levels.
        -r      the rows count of the labyrinth.
        -c      the cols count of the labyrinth.
KEYS SETTINGS
        ? - show keys settings menu;
        : - command mode;
        Space or m - toggle the map;
        ESC - put the game on pause;

        Moving:
        ↑ or j - move to the upper room;
        ↓ or k - move to the bottom room;
        ← or h - move to the left room;
        → or l - move to the right room;
        → or l - move to the right room;
</pre>

# How to:

 * build:

```
make
```

 * run:
```
make run
```

 * run tests:
```
make test
```

 * generate `compile_flags.txt` for `clangd`:
```
bear -- make test
```

