#pragma once

#include <cstdlib>
#include <cstring>

enum graph_input_type { seat = 'x', wall = '-', space = ' ' };

struct Graph
{
  int** adjacency_matrix;
  int size;
};

struct Node
{

  Node(int _g) : g(_g), size(0)
  {
    vertices = (int*)malloc(sizeof(int) * size);
  }
  Node(int _g, const Node& parent, int i) : g(_g), size(parent.size + 1)
  {
    vertices = (int*)malloc(sizeof(int) * size);
    memcpy(vertices, parent.vertices, sizeof(int) * parent.size);
    vertices[parent.size] = i;
  }

  int g;
  int size;
  int* vertices;
};