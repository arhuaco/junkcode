typedef struct NeuralUnit NeuralUnit;

/* create a neural unit */
NeuralUnit    * neural_unit_new         (void);

/* delete a neural unit */
void            neural_unit_destroy     (NeuralUnit *u);

/* initialize a neural unit */
void            neural_unit_init        (NeuralUnit *u,                                                                  double initial_state);

/* iterate the network and set the memory value */
void            neural_unit_iterate     (NeuralUnit *u);

/* make a neural unit send a signal to another neural unit */
void            neural_unit_signal      (const NeuralUnit *s,                                                            NeuralUnit *d, double weight);

/* set an input to be always present in the unit */
void            neural_unit_persistent_input_set                                                                        (NeuralUnit *u, double value);

/* send an input signal to the neural unit */
void           neural_unit_input_add     (NeuralUnit *u, double value);

/* get output from the neural unit */
double         neural_unit_output_get   (NeuralUnit *u);
