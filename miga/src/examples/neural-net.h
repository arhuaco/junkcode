typedef struct NeuralNet NeuralNet;

/* create a neural net */
NeuralNet        * neural_net_new       (int size);

/* destroy a neural net */
void               neural_net_destroy   (NeuralNet *n);

/* initialize a neural net */
void              neural_net_init       (NeuralNet *n, double initial_state);

/* iterate the neural net */
void              neural_net_iterate    (NeuralNet *n, double *weights);

/* set an input to be always present for a given unit */
void              neural_net_persistent_input_set                                                                       (NeuralNet *n, int who,                                                          double value);

/* send input to a given unit */
void             neural_net_input_send  (NeuralNet *n, int who,                                                          double value);

/* get output from a given unit */
double           neural_net_output_get  (NeuralNet *n, int who);
