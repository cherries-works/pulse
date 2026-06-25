The usage of Pulse has been kept to simple by default. It is not a replacement for
bigger tools, instead a tool that helps you to monitor your system in less than a
minute.

# Download

Clone the github repo, and run

```
    $ git clone https://github.com/cherries-works/pulse.git
    $ cd pulse/
    $ make
    $ ./dist/pulse
```

To see the metrics in your terminal.

# Commands
`
    $ ./dist/pulse --help      
`

```
    --port [number]      Determine the port where the website will be hosted (omits --web).
    --web                Hosts website (and API) on default port 8080.
    --sleep              How many seconds the program sleeps before updating (TUI only).
    --headless           Runs program without TUI (currently only useful with --web).
    --stop               Stops all of the processes that Pulse currently runs.
    --help               Prints this. 
```

`
    $ ./dist/pulse --web
`

To host a Web version of the monitoring system on port 8080 (by default).
To change the default port, use the `--port [number]` argument.

Add `--headless` argument to run it without the TUI in the background.
(Stop by running `./dist/pulse --stop`).

`
    $ ./dist/pulse --sleep 1
`

Pulse updates every 1 second by default. Enter a given number that update
per the entered number `--sleep 10 # updates ever 10 seconds`.
