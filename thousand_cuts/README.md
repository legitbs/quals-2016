# Thousand Cuts

Thousand cuts is a series of kind of basic buffer overflow challenges. In each
level, teams will be provided with a bundle of challenges and an address. The
listener at the address will give the client a binary name, and the client
submits a base-64 encoded string to segfault the given binary. After the client
segfaults the configured number of binaries, they get the flag.

## Levels

1. *stack* binaries have different buffer sizes and different canaries that
   must be represented in the exploit.
2. *vary-stack* binaries have all the differences of *stack* binaries, plus
   extra functions padding out the binary and changing offsets of magic numbers.
3. *scramble-stack* binaries have all the differences of *vary-stack* binaries,
   and also have their functions in random orders.

## Generating Binaries

Each level is generated independently, in a few steps. It requires both Ruby
(tested with 2.3), and a working CGC compilation stack (tested with the `cb`
Vagrant image provided by DARPA).

The first step is `generator.rb`, which takes two environment variables.
`ITERATIONS` controls the number of binaries to generate, and `SEED` controls
the PRNG seed.

It generates some of the `tmp/gen` directory tree:

* `buildall.sh` runs in the CGC `cb` machine. It builds all the binaries.
* `chkall.sh` runs `cb-test` on all the binaries.
* `chk` contains the scripts used by `chkall.sh`
* `mk` contains Makefile fragments used by `buildall.sh`
* `pov` contains XML POVs used by `chkall.sh`
* `raw` contains raw exploit strings

After `generator.rb` runs, we run `buildall.sh` inside the CGC `cb` machine.

It generates, again in `tmp/gen`:

* `cb` contains compiled challenge binaries. This should be bzipped and
  shipped to teams.

## Running

Each level has a `runner.py` that handles stdio chat with the client. It picks a
random set of binaries to challenge the client with, prompts them with the name
of a binary, fetches and decodes their base64-encoded string, runs the binary,
writes the string into it, and makes sure it segfaults. If the client takes too
long, or the binary doesn't segfault, it exits.

The runner can be configured with several environment variables.

* `FIRST_CHALLENGE_NAME` the easy runner always starts with the same binary,
  which users should have solved as a baby's first challenge. Mandatory for
  the easy level, doesn't exist on other levels.
* `INPUT_TIMEOUT` how many seconds to wait for user input. I'm slow as heck so I
  set it to 60 for testing. Defaults to 15 seconds.
* `CRASH_TIMEOUT` how many seconds to wait for the binary to segfault. The
  default 5 seconds is probably way slow but what if shit gets slow in prod idk
* `FLAG` each level defaults to a different flag, easily configurable.

## Testing in Production

`prod-test.sh` runs tests for all four levels, and is configured for 2016 quals
server names and ports.

You can run it with Docker!

```sh
docker build -t thousand_cuts:test .
docker run -it thousand_cuts:test
```

Each level has a `player.rb` script that runs three tests: too much input, input
that doesn't trigger a crash, and the full success flow for each service. This
script requires Ruby and Minitest, and the built up `tmp/gen/raw/*` directories
full of valid crash strings.

## <3 vito
