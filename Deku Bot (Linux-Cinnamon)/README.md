### Made With Simple and Fast Multimedia Library
[![SFML logo](https://www.sfml-dev.org/images/logo.png)](https://www.sfml-dev.org)

---

# Deku Chess Bot - Linux Cinnamon


The following files are a port of the source code for Deku implemented in Windows 10, for Windows 32-bit systems and developed in Visual Studio Community 2022. While originally Deku's objective was to view as deep as it could to find the best possible move, it quickly became appearent that no matter how long Deku searched, a large majority of moves were missed, leading to the second improved version; breadth-first minimax. Otherwise known as Iterative Deepening. While nodes will be revisited more often than depth-first minimax, Iterative Deepening ensures that it does not search too deep to undermine other parts of the tree. So while breadth-first won't search nearly as deep as depth-first, the quality of search is vastly improved. Something I noticed in the port is that it seems to underperform compared to its Windows counterpart. Where on the first move Deku averages a depth of roughly 20 on Windows, a depth of 7 is achieved in the same time on Linux. This could be because of my Virtual Machine, or the change in timer from difftime() to chrono. Regardless, Deku performs similarly.

In several games against [stockfish](https://listudy.org/en/play-stockfish) Iterative Deepening always yielded better results than depth-first with games lasting up to 20+ minutes (Note: Deku's Search Time was 1 minute). In contrast, depth-first set for the same time led to a quick checkmate by stockfish in the first few moves. There are improvements that can be made to Deku, such as a transposition table or sorting moves by most promising, but as of right now I am satisfied with its performance.

One other thing of note for Deku is that it balances quality of move with depth of search. Once Deku's maximum time has been reached, it begins to return invalid data to flag as such, meaning parts of the tree with potentially better moves are pruned. To combat this tradeoff, and the event that by fitness value the first move is considered the best, fitness is weighed by how deep in the tree that move was found. By doing this, it rewards Deku for deeper searches, but doesn't eliminate the possiblility of an instant win on one of the first moves.
