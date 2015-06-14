Guild of Thieves
===
This project solves distributed computing problem using [MPI](https://en.wikipedia.org/wiki/Message_Passing_Interface) protocol. It exists in imaginary world of Guild of Thieves. Problem was introduced by [Phd. Arkadiusz Danilecki](http://www.cs.put.poznan.pl/adanilecki/).

### Description
Thieves have rights for robbing aristocracy houses under several rules.

1. Each thief needs to find a pair before robbing a house. They can’t rob a house on their own.
2. Single house can be robbed by only one pair of thieves at time.
3. Before robbery thieves need to complete a lot of paper work. This action takes lots of time since thieves aren’t well educated. What is more, only S thieves can sign papers at the same time.
4. After being robbed, the house needs to bypassed for some time.

There are `H` houses and `T` thieves. `T` doesn’t have to be an even number. `T` > `H`, `T` > `S`. Relation between `H` and `S` is not known.
Guild of Thiefs


### Building
```
make
```

### Running
```
mpirun -default-hostfile none -np *number_of_processes* ./bin/main
```
