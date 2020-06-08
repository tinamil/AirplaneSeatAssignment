#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <algorithm>
#include <stack>
#include <queue>

enum graph_input_type { seat = 'x', wall = '-', space = ' ' };

struct Graph
{
  float** adjacency_matrix;
  int size;
};

struct Node
{

  Node(float _g) : g(_g), size(0)
  {
    vertices = (int*)malloc(sizeof(int) * size);
  }
  Node(float _g, const Node& parent, int i) : g(_g), size(parent.size + 1)
  {
    vertices = (int*)malloc(sizeof(int) * size);
    memcpy(vertices, parent.vertices, sizeof(int) * parent.size);
    vertices[parent.size] = i;
  }

  float g;
  int size;
  int* vertices;
};

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
  float** adjacency_matrix = new float* [vertex_counter];
  for(size_t i = 0; i < vertex_counter; ++i)
  {
    adjacency_matrix[i] = new float[vertex_counter];
    std::fill(adjacency_matrix[i], adjacency_matrix[i] + vertex_counter, 0.f);
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
              adjacency_matrix[index1][index2] = 1 + (float)rand() / RAND_MAX;
          }
        }
      }
    }
  }
  return Graph{ adjacency_matrix, vertex_counter };
}

Node search1(Graph graph, int n = 43)
{
  size_t expansions = 0;
  Node best{ INT_MAX };

  std::priority_queue<Node, std::vector<Node>, NodeSort> p_queue;
  p_queue.emplace(0.f);

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
      int end = graph.size + next.size - n + 1;
      for(int i = start; i < end; ++i)
      {
        float cost = 0;
        for(int j = next.size - 1; j >= 0; --j)
        {
          cost += graph.adjacency_matrix[i][next.vertices[j]];
        }
        if(next.g + cost < best.g)
        {
          if(next.size + 1 == n)
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

  return best;
}

int main(int argc, char* argv[])
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
      auto matrix = process_graph(graph_string);
      Node best = search1(matrix);
      std::cout << "Best solution = " << best.g << "; ";
      for(int i = 0; i < best.size; ++i)
      {
        std::cout << best.vertices[i] << " ";
      }
      std::cout << "\n";
    }
    else
    {
      std::cerr << "Failed to open " << argv[i];
      break;
    }
  }
  return 0;
}
