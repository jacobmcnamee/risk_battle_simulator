# Risk Battle Simulator

A command-line utility to simulate
[Risk](https://en.wikipedia.org/wiki/Risk_(game)) battles and compute probable
outcomes.

## Usage

### Simulation Mode

`risk sim <num_attackers> <num_defenders>`

Simulates a single Risk battle between `<num_attackers>` attacking armies and
`<num_defenders>` defending armies.

Example:
```
$ risk sim 7 3
simulating battle with 7 attackers vs. 3 defenders
results:
attacker loses 1 armies
defender loses 3 armies
attacker wins
```

### Probability Mode
`risk prob <num_attackers> <num_defenders> <runs>`

Computes probabilities for all possible outcomes of a Risk battle between
`<num_attackers>` attacking armies and `<num_defenders>` defending armies using
`<runs>` test runs.

Example:
```
$ risk prob 7 3 100000
calculating probabilities for 7 attackers vs. 3 defenders using 100000 runs
results:
attacker wins:
armies lost | probability | cumulative
  0            24.7%         24.7%
  1            20.6%         45.2%
  2            17.5%         62.7%
  3            13.2%         75.9%
  4             8.7%         84.6%
  5             4.4%         89.0%
  6             2.0%         91.0%
defender wins:
armies lost | probability | cumulative
  0             1.9%          1.9%
  1             4.3%          6.1%
  2             2.8%          9.0%
```
