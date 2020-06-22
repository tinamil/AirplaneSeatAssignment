#pragma once
#include "Graph.h"
#include <vector>

class CPLEX_Solver
{
public:
  std::vector<bool> Solve_Vertex_Packing_Risk_Minimization(const Graph& g, int target_seats_N);
  std::vector<bool> Solve_Risk_Constrained_Vertex_Packing(const Graph& g, double max_risk_R);
};