#include <fann.h>
#include <parallel_fann.h>
#include "candidate_db.h"
#include "hod_candidate.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {

  if (argc < 3) {
    printf("usage: train_classifier train_data.db result.nn\n");
    return 1;
  }

  const char* db_file = argv[1];
  const char* nn_file = argv[2];

  printf("%s %s\n", db_file, nn_file);

  candidate_db db;
  candidate_db_init(&db, db_file);

  const int features_length = 3780;
  int count = candidate_db_get_count(&db);

  float* features = (float*)calloc(count * features_length, sizeof(float));
  float* output = (float*)calloc(count, sizeof(float));

  hod_result* training_data = (hod_result*)calloc(count, sizeof(hod_result));

  for (int i = 0; i < count; i++) {
    training_data[i].num_features = features_length;
    training_data[i].features = &features[features_length * i];
  }

  candidate_db_get_features(&db, training_data, count);
  candidate_db_close(&db);

  for (int i = 0; i < count; i++) {
    float v = float(training_data[i].human * 2 - 1);
    output[i] = v;
  }

  free(training_data);

  fann* nn = fann_create_standard(2, features_length, 1);
  fann_set_activation_function_output(nn, FANN_SIGMOID_SYMMETRIC);

  fann_train_data* fann_td = fann_create_train_array(count, features_length, features, 1, output);
  
  for (int i = 1; i <= 5000; i++) {
    float err = fann_train_epoch_irpropm_parallel(nn, fann_td, 8);
    printf("Epoch: %d MSE %.10f\n", i, err);
    if (err < 0.0001f) break;
  }

  fann_save(nn, nn_file);

  fann_destroy_train(fann_td);
  fann_destroy(nn);

  return 0;
}
