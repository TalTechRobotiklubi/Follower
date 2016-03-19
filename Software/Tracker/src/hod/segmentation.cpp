#include "segmentation.h"
#include <algorithm>

// Code from: http://cs.brown.edu/~pff/segment/

inline float threshold_value(float size, float c) { return c / size; }

int segmentation_find(segmentation* u, int x) {
  int y = x;
  while (y != u->nodes[y].p) {
    y = u->nodes[y].p;
  }

  u->nodes[x].p = y;
  return y;
}

void segmentation_reset(segmentation* u) {
  u->num_nodes = u->num_initial_nodes;
  for (int i = 0; i < u->num_initial_nodes; i++) {
    u->nodes[i].size = 1;
    u->nodes[i].p = i;
  }
}

void segmentation_init(segmentation* u, int num_elements) {
  u->num_initial_nodes = num_elements;
  u->threshold = (float*)calloc(num_elements, sizeof(float));
  u->nodes = (seg_node*)calloc(num_elements, sizeof(seg_node));

  segmentation_reset(u);
}

void segmentation_destroy(segmentation* u) {
  free(u->nodes);
  free(u->threshold);
  free(u);
}

void segmentation_join(segmentation* u, int x, int y) {
  seg_node* nodes = u->nodes;
  if (nodes[x].rank > nodes[y].rank) {
    nodes[y].p = x;
    nodes[x].size += nodes[y].size;
  } else {
    nodes[x].p = y;
    nodes[y].size += nodes[x].size;

    if (nodes[x].rank == nodes[y].rank) {
      nodes[y].rank++;
    }
  }

  u->num_nodes--;
}

int segmentation_size(const segmentation* u, int x) { return u->nodes[x].size; }

void segment_graph(segmentation* u, edge* edges, int num_edges, float c,
                   int min_size) {
  std::sort(edges, edges + num_edges,
            [](const edge& a, const edge& b) { return a.weight < b.weight; });

  for (int i = 0; i < u->num_initial_nodes; i++) {
    u->threshold[i] = threshold_value(1.f, c);
  }

  for (int i = 0; i < num_edges; i++) {
    edge* e = &edges[i];

    int a = segmentation_find(u, e->a);
    int b = segmentation_find(u, e->b);

    if (a != b) {
      if (e->weight <= u->threshold[a] && e->weight <= u->threshold[b]) {
        segmentation_join(u, a, b);
        a = segmentation_find(u, a);
        u->threshold[a] =
            e->weight + threshold_value(segmentation_size(u, a), c);
      }
    }
  }

  for (int i = 0; i < num_edges; i++) {
    edge* e = &edges[i];
    int a = segmentation_find(u, e->a);
    int b = segmentation_find(u, e->b);
    if (a != b && (segmentation_size(u, a) < min_size ||
                   segmentation_size(u, b) < min_size)) {
      segmentation_join(u, a, b);
    }
  }
}
