#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include "edge.h"

struct seg_node {
  int rank;
  int p;
  int size;
};

struct segmentation {
  int num_initial_nodes;
  int num_nodes;
  float* threshold;
  seg_node* nodes;
};

int segmentation_find(segmentation* u, int x);
void segmentation_reset(segmentation* u);
void segmentation_init(segmentation* u, int num_elements);
void segmentation_destroy(segmentation* u);
void segmentation_join(segmentation* u, int x, int y);
int segmentation_size(const segmentation* u, int x);
void segment_graph(segmentation* u, edge* edges, int num_edges, float c,
                   int min_size);

#endif
