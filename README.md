# trading-bot-cpp

```bash
mkdir -p build && cd build
cmake -G Ninja .. && cmake --build .
ctest
```

## Plots for tests

To enable plotting for tests to see strategy performance you need `gnuplot` to be installed
<details>
    <summary>
        for apt (ubuntu/etc)
    </summary>
    <code>
    sudo apt update && sudo apt install gnuplot
    </code>
</details>

<details>
    <summary>
        for brew(mac)
    </summary>
    <code>
    brew install gnuplot
    </code>
</details>

<details>
    <summary>
        for windows
    </summary>
    http://www.gnuplot.info/
</details>
