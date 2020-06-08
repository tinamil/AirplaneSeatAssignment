#include "BranchAndBound.h"
#include <queue>
#include <iostream>

struct NodeSort
{
  bool operator()(const Node& a, const Node& b)
  {
    if(a.size == b.size)
    {
      return a.g > b.g;
    }
    else return a.size < b.size;
  }
};

std::vector<bool> BranchAndBound::Solve(Graph graph, int target_seats)
{
  size_t expansions = 0;
  Node best{ INT_MAX };

  std::priority_queue<Node, std::vector<Node>, NodeSort> p_queue;
  p_queue.emplace(0);

  while(!p_queue.empty())
  {
    Node next = p_queue.top();
    p_queue.pop();
    if(next.g < best.g)
    {
      expansions += 1;
      if(expansions % 10000000 == 0)
      {
        std::cout << expansions << "; ";
        for(int i = 0, j = std::min(5, next.size); i < j; ++i)
        {
          std::cout << next.vertices[i] << " ";
        }
        std::cout << "\n";
      }
      int start = next.size > 0 ? next.vertices[next.size - 1] + 1 : 0;
      int end = graph.size + next.size - target_seats + 1;
      for(int i = start; i < end; ++i)
      {
        int cost = 0;
        for(int j = next.size - 1; j >= 0; --j)
        {
          cost += graph.adjacency_matrix[i][next.vertices[j]];
        }
        if(next.g + cost < best.g)
        {
          if(next.size + 1 == target_seats)
          {
            best = Node(next.g + cost, std::ref(next), i);
            std::cout << "New best solution cost: " << best.g << "\n";
          }
          else
          {
            p_queue.emplace(next.g + cost, std::ref(next), i);
          }
        }
      }
    }
    free(next.vertices);
  }

  std::vector<bool> results(graph.size);
  std::fill(results.begin(), results.end(), false);
  for(size_t i = 0; i < results.size(); ++i)
  {
    for(size_t j = 0; j < best.size; ++j)
    {
      if(best.vertices[j] > i) break;
      if(best.vertices[j] == i) results[j] = true;
    }
  }
  return results;
}
