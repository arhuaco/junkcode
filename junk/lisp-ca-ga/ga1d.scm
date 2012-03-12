; This is a Genetic Algorithm with 1-dimensional
; binary genomes
;
; Jan 2006
; Nelson Castillo
; http://arhuaco.org
;
; I no longer like this program ! :-)
; It was useful, but some parts could be much better.
; Bear with me, this was the first functional Scheme program
; I wrote.

(require "fu-common.scm") 

; *********************************
; The binary genome
; It's 1-dimensional. We will use a vector for the representation.
;
; *********************************

; make a genome. Param can be the size of the genome of a sample genome.
; representation : (cons evaluation array)

(define (genome-make size)
  (cons 0.0 (make-vector size))) 

(define (genome-duplicate sample)
  (cons (genome-evaluation sample)
        (vector-duplicate (genome-vector sample))))

(define genome-vector cdr)

(define genome-evaluation car)

(define genome-evaluation-set! set-car!)

; procedures that operate on genomes

(define (genome-size g)
  (vector-length (genome-vector g)))

(define (genome-gene g i)
  (vector-ref (genome-vector g) i))

(define (genome-gene-set! g i new-value)
  (vector-set! (genome-vector g) i new-value))

; generic gene iterator

(define (genome-foreach-gene proc g)
  (let loop ((i 0))
    (if (< i (genome-size g))
        (begin 
          (proc i)
          (loop (++ i))))))

; Initializers

(define (genome-uniform-initializer! g)
  (genome-foreach-gene (lambda (i) 
                         (genome-gene-set! g i (random 2)))
                         g))
        
(define (make-genome-constant-initializer k)
  (lambda (g)
    (genome-foreach-gene (lambda (i) 
                           (genome-gene-set! g i k))
                           g)))

; Count 1's in the genome

(define (genome-count-1s g)
  (define count 0)
  (genome-foreach-gene (lambda (i)
                         (if (= (genome-gene g i)
                                1)
                             (inc! count)))
                       g)
  count)

; Mutation
; p-mut is the probability for the mutation of each gene

(define (genome-mutate g p-mut)
  (let ((new (genome-duplicate g)))
    (genome-foreach-gene (lambda (i)
                           (if (<= (random-real) p-mut)
                             (genome-gene-set! new i (- 1 (genome-gene new i))))) ; flip the i-th gene
                         new)
    new))

; Crossover

(define (genome-cross g1 g2)
  (let* ((size (genome-size g1))
         (new1 (genome-make size))
         (new2 (genome-make size))
         (cross-point (* (random-real) size)))
    (genome-foreach-gene (lambda (i)
                           (genome-gene-set! new1
                                             i
                                             (genome-gene (if (<= i cross-point) g1 g2) i)))
                         new1)
    (genome-foreach-gene (lambda (i)
                           (genome-gene-set! new2
                                             i
                                             (genome-gene (if (<= i cross-point) g2 g1) i)))
                         new2)

  (cons new1 new2)))

; *********************************
;
; The population. A set of genomes.
;
; *********************************

(define (population-make size sample-genome)
  (let loop ((i 0)
             (genomes null))
    (if (< i size)
        (loop (++ i)
              (cons (genome-duplicate sample-genome)
                    genomes))
        genomes)))

; selectors

(define population-scores  (lambda (pop) (map car pop)))
(define population-genomes (lambda (pop) (map cdr pop)))

; evaluate the population

(define (population-evaluate! pop genome-evaluator)
  (map (lambda (genome) (genome-evaluation-set! genome
                                                (genome-evaluator genome)))
       pop)
  pop)

; discard part of the population
; the returned population is sorted according to fitness

(define (population-discard pop discard%)
  (let* ((sorted-pop (list-sort pop (lambda (g1 g2) (< (genome-evaluation g1) (genome-evaluation g2)))))
         (forget-size (* discard% (length pop))))
    (let loop ((i 0)
               (trimmed-pop sorted-pop))
      (if (< i forget-size)
          (loop (++ i)
                (cdr trimmed-pop))
          (reverse trimmed-pop)))))

; return a list of scores and the average score

(define (population-stats pop)
  (let* ((scores (population-scores pop))
          (avg (list-average scores)))
    (cons scores
          (* avg 1.0))))

; *********************************
;
; The Genetic Algorithm.
;
; *********************************

; Make a procedure that will select a new parent when called.
; We can rewrite this to use an auxiliar vector and a binary search.

(define (make-parent-selector pop)
  (let ((total-fitness (accumulate 0 + (map genome-evaluation pop))))
    (lambda ()
      (let ((lottery (* (random-real) total-fitness))) 
        (let loop ((which pop)
                   (sum 0))
          (let ((new-sum (+ sum (genome-evaluation (car which)))))
            (if (or (null? (cdr which)) ; the last one
                    (<= lottery new-sum)) ; the chosen one
                (car which)
                (loop (cdr which)
                      new-sum))))))))

; Iterate a population to produce a new one.
; The population _must_ have been evaluated before.

(define (ga-iterate pop elitism? cross% mut% discard%)
  (let* ((pop-size (length pop))
         (survivors (population-discard pop discard%))
         (select-parent (make-parent-selector survivors)))
    (let loop ((i (if elitism? 1 0))
               (new-pop (if elitism?
                            (cons (car survivors) null)
                            null)))
      (if (< i pop-size)
          (let* ((parent-1 (select-parent))
                 (parent-2 (select-parent))
                 (parents (if (<= (random-real) cross%)                              
                              (genome-cross parent-1 parent-2) 
                              (cons parent-1 parent-2)))
                 (child-1 (genome-mutate (car parents) mut%))
                 (child-2 (genome-mutate (cdr parents) mut%)))
            (loop (+ i 2)
                  (cons child-1
                        (cons child-2 new-pop))))      
          new-pop))))

; The genetic algorithm. Contains the population and a dispatcher.

(define (ga-make pop-size genome-size genome-initializer! genome-evaluator elitism? cross% mut% discard%)                 
  
  (define pop (population-make pop-size (genome-make genome-size))) ; mutable data!
  (map genome-initializer! pop)
  (population-evaluate! pop genome-evaluator)
 
  (lambda (action)
    (cond ((eq? action 'pop) pop)
          ((eq? action 'step)
             (set! pop
                   (population-evaluate! (ga-iterate pop elitism? cross% mut% discard%)

                                         genome-evaluator))))))
; Iterate the GA a given number of steps

(define (ga-iterate! ga steps)
  (let loop ((i 0))
    (if (< i steps)
        (begin (ga 'step)
               (loop (++ i)))
        ga)))

; Print statistics about the population

(define (ga-print-stats ga)
  (let* ((stats (population-stats (ga 'pop)))
         (scores (car stats))
         (avg (cdr stats)))
    (write "scores => ")
    (write scores)
    (write " average => ")
    (write (* 1.0 avg)) ; we don't want to print exact numbers (3 1/2, for instance)
    (newline)))


; *********************************
;
; Tests :)
;
; Evolve with the classic example that uses the number of 1s in the genome
; as the fitness for the evolution.
;
; *********************************


(define (ga-test)
  (define ga (ga-make 10 10 genome-uniform-initializer! genome-count-1s #t 0.9 0.05 0.2))
  (ga-iterate! ga 100) ; iterate 100 steps
  (display (population-genomes (ga 'pop)))  ; print the final population (only the genomes)
  (newline)
  (ga-print-stats ga))

; (ga-test)
