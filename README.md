# Minimal Feedback Arc Set 

The algorithm removes cycles in a directed graph by removing the least edges possible. A set of edges that must be removed to make a graph acyclic is also called a **feedback arc set**; and the set with the least edges is a **minimal feedback arc set**.

## Implementation

There are two programs: a **generator** program and a **supervisor** program. 

Multiple generator processes generate random solutions to the problem and report their solutions to one supervisor process.
The supervisor process remembers the best solution so far. The processes communicate with each other by means of a circular buffer, which is implemented using shared semaphores and a shared memory.

## Example

![image](https://github.com/r-gg/fb_arc_set/assets/90387385/1b77a8a4-8046-45be-9117-7324643cb8a9)

### Invocation

![image](https://github.com/r-gg/fb_arc_set/assets/90387385/5d3d6271-c2ce-4db4-84b8-27f65b0b017d)
