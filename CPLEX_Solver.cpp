#include "CPLEX_Solver.h"

decltype (auto) CPLEX_Solver::SetupParams(IloEnv& env)
{
  IloCplex::ParameterSet params(env);
  params.setParam(IloCplex::Param::ClockType, 1); //CPU time
  params.setParam(IloCplex::Param::TimeLimit, 21600); //6 hours of CPU time 
  params.setParam(IloCplex::Param::WorkMem, 102400); //100 GB

  //Enable these parameters to get all solutions instead of 1st optimal
  //params.setParam(IloCplex::Param::MIP::Pool::AbsGap, 0); 
  //params.setParam(IloCplex::Param::MIP::Pool::Intensity, 4); 
  //params.setParam(IloCplex::Param::MIP::Limits::Populate, 2100000000); 

  return params;
}

std::vector<bool> CPLEX_Solver::Solve_Vertex_Packing_Risk_Minimization(const Graph& g, const int target_seats)
{
  IloEnv              env;
  IloModel    model(env);

  IloInt const nbSeats = g.size;
  IloNum nbTargetSeats = target_seats;
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

  for(const auto& seat : g.filled_seats) {
    model.add(assigned_seats[seat] == 1);
  }

  model.add(IloMinimize(env, IloScalProd(shared_seats, edge_costs)));

  IloCplex  cplex(model);
  cplex.setParameterSet(SetupParams(env));
  cplex.solve();
  cplex.out() << "Solution status: " << cplex.getStatus() << std::endl;
  cplex.out() << "Optimal value: " << cplex.getObjValue() << std::endl;
  std::vector<bool> results;
  for(int i = 0; i < g.size; ++i)
  {
    int num = (int)round(cplex.getValue(assigned_seats[i]));
    if(num == 1) {
      results.push_back(true);
    }
    else {
      if(num != 0) std::cout << i << " " << num << " ERROR: non-zero/non-one seat assignment.\n";
      results.push_back(false);
    }
  }
  env.end();
  return results;
}

std::vector<bool> CPLEX_Solver::Solve_Risk_Constrained_Vertex_Packing(const Graph& g, double max_risk_R)
{
  IloEnv              env;
  IloModel    model(env);

  IloInt const nbSeats = g.size;
  IloNum const nbMaxRisk = max_risk_R;
  IloIntVarArray  assigned_seats(env, nbSeats, 0, 1);
  IloIntVarArray  shared_seats(env, nbSeats * nbSeats, 0, 1);
  IloIntArray      edge_costs(env, nbSeats * nbSeats);

  for(size_t row = 0; row < g.size; ++row)
  {
    for(size_t col = 0; col < g.size; ++col)
    {
      edge_costs[row * nbSeats + col] = g.adjacency_matrix[row][col];
      model.add(shared_seats[row * nbSeats + col] >= assigned_seats[row] + assigned_seats[col] - 1);
      model.add(shared_seats[row * nbSeats + col] >= 0);
    }
  }

  model.add(IloScalProd(shared_seats, edge_costs) <= nbMaxRisk);
  model.add(IloMaximize(env, IloSum(assigned_seats)));

  IloCplex  cplex(model);
  cplex.setParameterSet(SetupParams(env));
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