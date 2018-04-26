## Cold Filter

### Note
[Additional code for Journal version](https://github.com/cortalo/PersistentItem) 

### Introduction

Approximate stream processing algorithms, such as Count-Min sketch, Space-Saving, etc., support numerous applications in databases, storage systems, networking, and other domains. Unfortunately, because of the unbalanced distribution in real data streams, existing algorithms can hardly achieve small memory usage, fast processing speed, and high accuracy at the same time. To address this gap, we propose a meta-framework, called Cold Filter (CF), that enables faster and more accurate stream processing. 

Different from existing filters that mainly focus on hot items, our filter captures cold items in the first stage, and hot items in the second stage. Also, existing filters require two-direction communication – with frequent exchanges between the two stages; our filter on the other hand is one-direction – each item enters one stage at most once. Our filter can accurately estimate both cold and hot items, giving it a genericity that makes it applicable to many stream processing tasks. To illustrate the benefits of our filter, we deploy it on three typical stream processing tasks and experimental results show speed improvements of up to 4.7 times, and accuracy improvements of up to 51 times.

### About this repo

This repo contains all the algorithms in our experiments, as shown in the following table.

| Task                      | Algorithms                               |
| ------------------------- | ---------------------------------------- |
| Estimating item frequency | cm sketch (count min),  cm-cu sketch (count min sketch with conservative update), A sketch |
| Finding top-k hot items   | cm sketch with heap, cm-cu sketch with heap, space saving |
| Detecting heavy changes   | FlowRadar (invertible IBLT)              |

This repo also contains a small demo to show how to use this algorithms with a small dataset.

### Requirements

- The gather-and-report part of CF use SIMD instructions to achieve high speed, so the cpu must support SSE2 instruction set.
- cmake >= 2.6
- g++ (MSVC is not supported currently.)

### How to build

The project is built upon [cmake](https://cmake.org/). You can use the following commands to build and run.

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ../
./demo
```

### Related paper
[Cold Filter: A Meta-Framework for Faster and More Accurate Stream Processing](http://www.zhouy.me/paper/cf-sigmod18.pdf)
