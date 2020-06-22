#include "Graph.h"
#include "CPLEX_Solver.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>

Graph process_graph(std::vector<std::string> graph_strings)
{
//Build a mapping from seat row/column to vertex index for adjacency matrix position
  std::vector<std::vector<int>> vertex_indices;
  vertex_indices.resize(graph_strings.size());

  int vertex_counter = 0;
  for(size_t row = 0; row < graph_strings.size(); ++row)
  {
    vertex_indices[row].resize(graph_strings[row].length());
    std::fill(vertex_indices[row].begin(), vertex_indices[row].end(), -1);
    for(size_t col = 0; col < graph_strings[row].length(); ++col)
    {
      auto letter = graph_strings[row][col];
      if(letter == seat)
      {
        vertex_indices[row][col] = vertex_counter++;
      }
    }
  }

  //Build an n^2 adjacency matrix of integer values 
  int** adjacency_matrix = new int* [vertex_counter];
  for(size_t i = 0; i < vertex_counter; ++i)
  {
    adjacency_matrix[i] = new int[vertex_counter];
    std::fill(adjacency_matrix[i], adjacency_matrix[i] + vertex_counter, 0);
  }

  //Add the edges
  for(int row = 0; row < graph_strings.size(); ++row)
  {
    for(int col = 0; col < graph_strings[row].length(); ++col)
    {
      char letter = graph_strings[row][col];
      int index1 = vertex_indices[row][col];
      if(letter == seat)
      {
        int max_row_offset = 2;
        int max_col_offset = 2;
        int max_risk = 100;

        for(int row2 = row - max_row_offset; row2 <= row + max_row_offset; ++row2)
        {
          if(row2 < 0 || row2 >= vertex_indices.size()) continue;
          for(int col2 = col - max_col_offset; col2 <= col + max_col_offset; ++col2)
          {
            if(col2 < 0 || col2 >= vertex_indices[row2].size()) continue;
            if(vertex_indices[row2][col2] == -1) continue;
            int index2 = vertex_indices[row2][col2];
            // Assign edge weights based on relative placement of seats
            if(index1 != index2)
            {
              if(abs(row - row2) == 1 && col == col2)
              {
                adjacency_matrix[index1][index2] = max_risk;
              }
              else if(abs(row - row2) <= 1 && abs(col - col2) <= 1)
              {
                adjacency_matrix[index1][index2] = max_risk / 10;
              }
              else
              {
                adjacency_matrix[index1][index2] = max_risk / 100;
              }
              // std::cout << "Edge between (" << row << ", " << col << ") and (" << row2 << ", " << col2 << "): " << adjacency_matrix[index1][index2] << "\n";
            }
          }
        }
      }
    }
  }
  return Graph{ adjacency_matrix, vertex_counter };
}

void Display(std::vector<std::string> graph_strings, std::vector<bool> included_seats)
{
  std::cout << "\n";
  int seat = 0;
  for(size_t row = 0; row < graph_strings.size(); ++row)
  {
    for(size_t col = 0; col < graph_strings[row].size(); ++col)
    {
      if(graph_strings[row][col] == graph_input_type::seat)
      {
        if(seat >= included_seats.size()) return;
        if(included_seats[seat++]) std::cout << "x";
        else std::cout << "o";
      }
      else
      {
        std::cout << graph_strings[row][col];
      }
    }
    std::cout << "\n";
  }
  std::cout << "\n";
}

// Print usage message and throw exception.
static void usage(const char* progname)
{
  using namespace std;
  cerr << "Usage: " << progname << " ";
  cerr << "-n [number_of_seats_to_assign or risk constraint] ";
  cerr << "-m [model_type, 1 = risk minimize, 2 = risk constrained] ";
  cerr << "[input graph file] " << endl;
  cerr << " Exiting..." << endl;
}

double parse_num(const char* string)
{
  std::stringstream strValue;
  strValue << string;
  double value;
  strValue >> value;
  if(strValue.fail()) return -1;
  return value;
}


int main(int argc, char* argv[])
{
  CPLEX_Solver x;
  double model_input = -1;
  int model_type = -1;
  for(int i = 1; i < argc; ++i)
  {
    if(argv[i][0] == '-')
    {
      switch(argv[i][1])
      {
        case 'R':
        case 'r':
        case 'N':
        case 'n':
          model_input = parse_num(argv[i + 1]);
          i += 1;
          break;
        case 'M':
        case 'm':
          model_type = (int)round(parse_num(argv[i + 1]));
          i += 1;
          if(model_type < 1 || model_type > 2) usage(argv[0]);
          break;
        default: usage(argv[0]);
      }
    }
    else
    {
      if(model_input == -1)
      {
        usage(argv[0]);
        break;
      }
      std::ifstream graph_file(argv[i], std::ios::in);
      if(graph_file.is_open())
      {
        std::vector<std::string> graph_string;
        for(std::string line; std::getline(graph_file, line); )
        {
          graph_string.push_back(line);
        }
        auto matrix = process_graph(graph_string);
        std::vector<bool> solutions;
        switch(model_type)
        {
          case 1:
            solutions = x.Solve_Vertex_Packing_Risk_Minimization(matrix, (int)round(model_input));
            break;
          case 2:
            solutions = x.Solve_Risk_Constrained_Vertex_Packing(matrix, model_input);
            break;
        }
        Display(graph_string, solutions);
      }
      else
      {
        std::cerr << "Failed to open " << argv[i];
        break;
      }
    }
  }
  return 0;
}
