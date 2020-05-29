#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>
#include <stack>

enum graph_input_type { seat = 'x', wall = '-', space = ' ' };

struct Graph {
  uint8_t** adjacency_matrix;
  int size;
};


Graph process_graph(std::vector<std::string> graph_strings) {
  //Build a mapping from seat row/column to vertex index for adjacency matrix position
  std::vector<std::vector<int>> vertex_indices;
  vertex_indices.resize(graph_strings.size());

  int vertex_counter = 0;
  for (size_t row = 0; row < graph_strings.size(); ++row) {
    vertex_indices[row].resize(graph_strings[row].length());
    std::fill(vertex_indices[row].begin(), vertex_indices[row].end(), -1);
    for (size_t col = 0; col < graph_strings[row].length(); ++col) {
      auto letter = graph_strings[row][col];
      if (letter == seat) {
        vertex_indices[row][col] = vertex_counter++;
      }
    }
  }

  //Build an n^2 adjacency matrix of integer values 
  uint8_t** adjacency_matrix = new uint8_t * [vertex_counter];
  for (size_t i = 0; i < vertex_counter; ++i) {
    adjacency_matrix[i] = new uint8_t[vertex_counter];
    std::fill(adjacency_matrix[i], adjacency_matrix[i] + vertex_counter, 0);
  }

  //Add the edges
  for (int row = 0; row < graph_strings.size(); ++row) {
    for (int col = 0; col < graph_strings[row].length(); ++col) {
      auto letter = graph_strings[row][col];
      auto index1 = vertex_indices[row][col];
      if (letter == seat) {
        for (int row2 = row - 1; row2 <= row + 1; ++row2) {
          if (row2 < 0 || row2 >= vertex_indices.size()) continue;
          for (int col2 = col - 1; col2 <= col + 1; ++col2) {
            if (col2 < 0 || col2 >= vertex_indices[row2].size()) continue;
            if (vertex_indices[row2][col2] == -1) continue;
            auto index2 = vertex_indices[row2][col2];
            if (index1 != index2)
              adjacency_matrix[index1][index2] = 1;
          }
        }
      }
    }
  }
  return Graph{ adjacency_matrix, vertex_counter };
}

struct Node {

  Node(int _g) : g(_g) {}
  Node(int _g, std::vector<int> _v) : g(_g), vertices(_v) {}
  Node(int _g, std::vector<int> _v, int i) : g(_g), vertices(_v) {
    vertices.push_back(i);
  }

  int g;
  std::vector<int> vertices;
};

Node search1(Graph graph, int n = 42) {
  std::vector<int> included_vertices;
  included_vertices.resize(graph.size);
  int expansions = 0;
  Node best{ INT_MAX };

  Node s = Node{ 0, std::vector<int>() };
  std::stack<Node> stack;
  stack.push(s);

  while (!stack.empty()) {
    Node next = stack.top();
    stack.pop();
    expansions += 1;
    if (expansions % 100000 == 0) std::cout << expansions << "\n";
    for (int i = 0; i < graph.size; ++i) {
      bool already_included = false;
      for (int j = 0; j < next.vertices.size(); ++j) {
        if (next.vertices[j] == i) {
          already_included = true;
          break;
        }
      }
      if (already_included) continue;
      int cost = 0;
      for (int j = 0; j < next.vertices.size(); ++j) {
        cost += graph.adjacency_matrix[i][next.vertices[j]];
      }
      if (next.vertices.size() + 1 == n) {
        if (next.g + cost < best.g) {
          best = Node(next.g + cost, next.vertices, i);
          std::cout << "New best solution cost: " << best.g << "\n";
        }
      }
      else if (next.g + cost < best.g) {
        stack.emplace(next.g + cost, next.vertices, i);
      }
    }
  }

  return best;
}

int main(int argc, char* argv[])
{
  for (int i = 1; i < argc; ++i) {
    std::ifstream graph_file(argv[i], std::ios::in);
    if (graph_file.is_open())
    {
      std::vector<std::string> graph_string;
      for (std::string line; std::getline(graph_file, line); )
      {
        graph_string.push_back(line);
      }
      auto matrix = process_graph(graph_string);
      Node best = search1(matrix);
      std::cout << "Best solution = " << best.g << "; ";
      for (auto i : best.vertices) {
        std::cout << i << " ";
      }
      std::cout << "\n";
    }
    else {
      std::cerr << "Failed to open " << argv[i];
      break;
    }
  }
  return 0;
}
