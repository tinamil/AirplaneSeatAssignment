#include "Graph.h"
#include "CPLEX_Solver.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>

enum Model
{
  Coughing = 1, NotCoughing = 2
};

Graph process_graph(std::vector<std::string> graph_strings, Model model)
{
  //Build a mapping from seat row/column to vertex index for adjacency matrix position
  std::vector<std::vector<int>> vertex_indices;
  vertex_indices.resize(graph_strings.size());

  std::vector<int> filled_seats;

  int vertex_counter = 0;
  for(size_t row = 0; row < graph_strings.size(); ++row)
  {
    vertex_indices[row].resize(graph_strings[row].length());
    std::fill(vertex_indices[row].begin(), vertex_indices[row].end(), -1);
    for(size_t col = 0; col < graph_strings[row].length(); ++col)
    {
      auto letter = graph_strings[row][col];
      if(letter == seat || letter == empty)
      {
        if(letter == seat) filled_seats.push_back(vertex_counter);
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
      if(letter == seat || letter == empty)
      {
        int max_row_offset = 1;
        int max_col_offset = 4;

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
                switch(model)
                {
                  case Coughing: adjacency_matrix[index1][index2] = 58; break;
                  case NotCoughing: adjacency_matrix[index1][index2] = 90; break;
                }
              }
              else if(abs(row - row2) <= 1 && abs(col - col2) <= 1)
              {
                switch(model)
                {
                  case Coughing: adjacency_matrix[index1][index2] = 7; break;
                  case NotCoughing: adjacency_matrix[index1][index2] = 90; break;
                }
              }
              else if(abs(row - row2) <= 1 && abs(col - col2) <= 2)
              {
                switch(model)
                {
                  case Coughing: adjacency_matrix[index1][index2] = 4; break;
                  case NotCoughing: adjacency_matrix[index1][index2] = 90; break;
                }
              }
              else if(abs(row - row2) <= 1 && abs(col - col2) <= 4)
              {
                switch(model)
                {
                  case Coughing: adjacency_matrix[index1][index2] = 4; break;
                  case NotCoughing: adjacency_matrix[index1][index2] = 0; break;
                }
              }
              else
              {
                std::cout << "Error, did not assign an edge weight correctly";
              }
              // std::cout << "Edge between (" << row << ", " << col << ") and (" << row2 << ", " << col2 << "): " << adjacency_matrix[index1][index2] << "\n";
            }
          }
        }
      }
    }
  }
  return Graph{adjacency_matrix, vertex_counter, filled_seats};
}

void Display(std::vector<std::string> graph_strings, std::vector<bool> included_seats)
{
  std::cout << "\n";
  int seat = 0;
  for(size_t row = 0; row < graph_strings.size(); ++row)
  {
    for(size_t col = 0; col < graph_strings[row].size(); ++col)
    {
      if(graph_strings[row][col] == graph_input_type::seat || graph_strings[row][col] == graph_input_type::empty)
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
  cerr << "-t [risk model, 1 = coughing (default), 2 = not-coughing]";
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
  Model risk_model = Model::Coughing;
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
        case 't':
        case 'T':
          risk_model = (Model)round(parse_num(argv[i + 1]));
          i += 1;
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
        auto matrix = process_graph(graph_string, risk_model);
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
        int count = 0;
        for(const auto& x : solutions) {
          if(x) count += 1;
        }
        if(model_type == 1 && count != model_input) {
          std::cout << "ERROR: Too many seats assigned.\n";
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

int main2(int argc, char* argv[])
{
  for(int i = 1; i < argc; ++i)
  {
    std::ifstream graph_file(argv[i], std::ios::in);
    if(graph_file.is_open())
    {
      std::vector<std::string> graph_string;
      for(std::string line; std::getline(graph_file, line); )
      {
        graph_string.push_back(line);
      }
      auto matrix = process_graph(graph_string, Model::Coughing);
      int risk = 0;
      for(int i = 0; i < matrix.filled_seats.size(); ++i) {
        int i_seat = matrix.filled_seats[i];
        for(int j = i + 1; j < matrix.filled_seats.size(); ++j) {
          int j_seat = matrix.filled_seats[j];
          risk += matrix.adjacency_matrix[i_seat][j_seat] + matrix.adjacency_matrix[j_seat][i_seat];
        }
      }
      std::cout << risk << "\n";
    }
    else
    {
      std::cerr << "Failed to open " << argv[i];
      break;
    }
  }
  return 0;
}
