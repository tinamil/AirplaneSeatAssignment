#include "CPLEX_Solver.h"
#include <ilcplex/ilocplex.h>

std::vector<bool> CPLEX_Solver::Solve(Graph g, const int target_seats)
{
  IloEnv              env;
  IloModel    model(env);

  IloInt const nbSeats = g.size;
  IloInt const nbTargetSeats = target_seats;
  IloIntVarArray  assigned_seats(env, nbSeats, 0, 1);
  IloIntVarArray  shared_seats(env, nbSeats * nbSeats, 0, 1);
  IloIntArray      edge_costs(env, nbSeats * nbSeats);


  model.add(IloSum(assigned_seats) == nbTargetSeats);

  for(size_t row = 0; row < g.size; ++row)
  {
    for(size_t col = 0; col < g.size; ++col)
    {
      edge_costs[row * nbSeats + col] = g.adjacency_matrix[row][col];
      model.add(shared_seats[row * nbSeats + col] >= assigned_seats[row] + assigned_seats[col] - 1);
      model.add(shared_seats[row * nbSeats + col] >= 0);
    }
  }

  model.add(IloMinimize(env, IloScalProd(shared_seats, edge_costs)));

  IloCplex  cplex(model);
  cplex.solve();
  cplex.out() << "Solution status: " << cplex.getStatus() << std::endl;
  cplex.out() << "Optimal value: " << cplex.getObjValue() << std::endl;
  std::vector<bool> results;
  for(int i = 0; i < g.size; ++i)
  {
    results.push_back(cplex.getValue(assigned_seats[i]));
  }
  env.end();
  return results;
}