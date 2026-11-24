# <$PROJECT_NAME>

!Please add you project description here

## Getting Started


Type the following to compile the default variant <$VARIANT>:
```
make
```


The following variants are currently supported:
* <$VARIANT>
* --
* --
* --

To lint your default variant you can type:
```
make lint
```

The following targets are currently supported
* link (default)
* lint
* clean
* init  (just to initialize a naked project)


### Prerequisites

You need a valid compiler license


### Multiprocessing

Make supports multiprocessing. To speed-up your build type e.g.:
```
make lint -j
```

To define the number of processes:
```
make lint --jobs=4
```

Usually on a quad core system --jobs=8 is optimum. On a dual core system use --jobs=4.