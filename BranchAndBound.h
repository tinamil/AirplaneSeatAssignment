#pragma once

#include "Graph.h"
#include <vector>

class BranchAndBound
{
  std::vector<bool> Solve(Graph g, int target_seats);
};

