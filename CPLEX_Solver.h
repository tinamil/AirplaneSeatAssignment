#pragma once
#include "Graph.h"
#include <vector>

class CPLEX_Solver
{
public:
  std::vector<bool> Solve(Graph g, int target_seats);
};