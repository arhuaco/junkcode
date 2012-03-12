#ifndef __GA__PARAMS__H__
#define __GA__PARAMS__H__

typedef struct GAParams GAParams;

/* The evaluation function */
typedef REAL (*GAEvaluator) (GAGene* g);

GAParams *     ga_params_new               (void);
void           ga_params_delete            (GAParams *p);
void           ga_params_pmut_set          (GAParams *p, float pmut);
void           ga_params_pcross_set          (GAParams *p, float pcross);
void           ga_params_elitesize_set     (GAParams *p, unsigned int size);
void           ga_params_cachefitness_set  (GAParams* p, unsigned int cache);
float          ga_params_pmut_get          (GAParams *p);
float          ga_params_pcross_get          (GAParams *p);
unsigned int   ga_params_elitesize_get     (GAParams *p);
unsigned int   ga_params_cachefitness_get  (GAParams* p);

GAEvaluator    ga_params_evaluator_get     (GAParams *p);
void           ga_params_evaluator_set     (GAParams *p, GAEvaluator f);


#endif
