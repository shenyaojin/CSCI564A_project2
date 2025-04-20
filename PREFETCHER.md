1. Describe how your prefetcher works.

**Answer**: The implemented custom prefetcher operates based on a simple next-line prefetch strategy triggered exclusively on cache misses. Whenever a cache miss occurs, the prefetcher fetches the immediately next sequential cache line, anticipating future memory access patterns that exhibit sequential locality.

2. Explain how you chose that prefetch strategy.

**Answer**: This strategy was selected based on its simplicity, efficiency, and widespread use in common workloads, which often exhibit sequential access patterns. Prefetching only on misses helps limit bandwidth usage and prevents unnecessary prefetches, making it efficient for scenarios where sequential accesses frequently follow misses.

3. Discuss the pros and cons of your prefetch strategy.

**Pros**:

- Simple to implement and low computational overhead.

- Efficient for workloads with strong sequential memory access patterns.

- Limits memory bandwidth usage by prefetching selectively.

**Cons**:

- Ineffective for workloads with predominantly random or non-sequential memory access patterns.

- Limited predictive accuracy for complex memory access patterns.

- May cause cache pollution if prefetched lines are rarely or never accessed subsequently.


4. Demonstrate that the prefetcher could be implemented in hardware (this can be
   as simple as pointing to an existing hardware prefetcher using the strategy
   or a paper describing a hypothetical hardware prefetcher which implements
   your strategy).

**Answer**: The custom prefetch strategy used here is very similar to simple next-line hardware prefetchers already implemented in commercial processors. For instance, the Intel Core series processors commonly employ a next-line prefetcher strategy to exploit sequential locality. These hardware prefetchers are well-documented in literature and widely implemented due to their balance between complexity, cost, and performance.

5. Cite any additional sources that you used to develop your prefetcher.

Tourino, J., & Doallo, R. (2001). Adaptive stream buffers: A technique for compiler-controlled data prefetching. In Proceedings of the 2001 ACM SIGPLAN Workshop on Optimization of Middleware and Distributed Systems (pp. 37-46).