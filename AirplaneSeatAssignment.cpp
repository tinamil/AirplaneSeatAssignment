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
        for(int row2 = row - 1; row2 <= row + 1; ++row2)
        {
          if(row2 < 0 || row2 >= vertex_indices.size()) continue;
          for(int col2 = col - 1; col2 <= col + 1; ++col2)
          {
            if(col2 < 0 || col2 >= vertex_indices[row2].size()) continue;
            if(vertex_indices[row2][col2] == -1) continue;
            int index2 = vertex_indices[row2][col2];
            if(index1 != index2)
              adjacency_matrix[index1][index2] = 1;
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
  cerr << "Usage: " << progname << "-n [number_of_seats_to_assign] [input graph file]" << endl;
  cerr << " Exiting..." << endl;
}

int parse_int(const char* int_string)
{
  std::stringstream strValue;
  strValue << int_string;
  unsigned int intValue;
  strValue >> intValue;
  if(strValue.fail()) return -1;
  return intValue;
}

int main(int argc, char* argv[])
{
  CPLEX_Solver x;
  int num_seats = -1;
  for(int i = 1; i < argc; ++i)
  {
    if(argv[i][0] == '-')
    {
      switch(argv[i][1])
      {
        case 'N':
        case 'n':
          num_seats = parse_int(argv[i + 1]);
          i += 1;
          break;
        default: usage(argv[0]);
      }
    }
    else
    {
      if(num_seats == -1)
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
        auto solution = x.Solve(matrix, num_seats);
        Display(graph_string, solution);
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
