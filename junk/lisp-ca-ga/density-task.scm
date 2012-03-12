;
; Density Task
;
; Goal : To find a cellular automaton that decides whether or not the
; initial configuration contains a majority of 1s [1].
; 
; Nelson Castillo
; http://arhuaco.org
;
; [1] http://www.santafe.edu/~evca/Papers/handbook-of-ec.ps

(load "ca1d.scm")
(load "ga1d.scm")

; Auxiliar functions

(define (make-integer-set start end increment)
  (let loop ((list null)
             (i end))
    (if (>= i start)
        (loop (cons i list)
              (-- i))
        list)))


(define (vector-fill-random! vector symbol how-many)
  (let ((positions (list->vector (make-integer-set 0 (-- (vector-length vector)) 1))))
    (let loop ((count how-many)
               (positions-last-pos (-- (vector-length positions))))
      (if (> count 0)
          (let ((random-pos (inexact->exact (truncate (* (++ positions-last-pos) (random-real))))))
            (vector-set! vector (vector-ref positions random-pos) symbol)
            (vector-set! positions random-pos (vector-ref positions positions-last-pos))
            (loop (-- count)
                  (-- positions-last-pos)))
          vector))))

(define (test-vector-fill-random)
  (let ((v (make-vector 10 0)))
    (vector-fill-random! v 1 2)
    (if (= (vector-count-ocurrences v 1 =) 2)
        #t
        #f)))


; *********************************
;
; Compute the task
;
; *********************************

(define (density-of-1s v)
  (/ (vector-count-ocurrences v 1 =)
     (vector-length v)))

; Let's create the training set.
; width should be an even number.

(define (training-set-make width set-size)
  
  (define (make-one zero-or-1)
    (vector-fill-random! (make-vector width (- 1 zero-or-1))
                         zero-or-1
                         (+ 1 (random (quotient width 2)))))
  
  (assert (= (remainder set-size 2) 0)
          "training-set-make : set must be even in size")
  
  (assert (= (remainder width 2) 1)
          "training-set-make : width must be odd in size")
  
  (let loop ((i 0)
             (result null))
    (if (< i set-size)
        (loop (++ i)
              (cons (make-one (remainder i 2))
                    result))
        result)))

(define (test-training-set-make)
  (let* ((test-set-size 20)
         (vector-size 21)
         (densities
          (map density-of-1s
               (training-set-make vector-size test-set-size)))
         (half (accumulate 0
                           (lambda (x y)
                             (cond ((> y 0.5) (+ x 1))
                                   ((< y 0.5) x)
                                   (else (assert #f "density = 0.5 in test-training-set-make"))))
                           densities)))
    (if (= half (/ test-set-size 2)) #t #f)))

; *********************************
;
; The fitness of the genetic algorithm
;
; *********************************

; This function maps a genome to an integer. Note that the
; representation (genome) is a binary number that starts
; with the less significant bit.

(define (genome->rule g)
  (let* ((v (genome-vector g))
         (size (vector-length v)))
    (let loop ((i 0)
               (mul 1)
               (sum 0))
      (if (< i size)
          (loop (++ i)
                (* 2 mul)
                (+ sum
                   (if (= (vector-ref v i) 1) mul 0)))
          sum))))

(define (test-rule ca maximum-ca-iterations initial-configuration density>0.5?)
 
  (define (partial-credit n-ones density>0.5?)
    (define (punish partial)
      (if (< partial 0.5)
          0
          0.1))
   
    (let* ((density (/ n-ones (ca-grid-size (ca-grid ca)))))
      (if density>0.5?
          (punish density)
          (punish (- 1 density)))))
   
  
  (assert (= (ca-grid-size (ca-grid ca))
             (vector-length initial-configuration))
          "size of vector doesn't match in test-rule")
  
  (let* ((grid (ca-grid ca))
         (size (ca-grid-size grid)))
    
    ; Initialize the CA grid. Side effects are evil. Get rid of them!
    
    (let loop ((i 0))
      (if (< i size)
          (begin (ca-grid-cell-set! grid i (vector-ref initial-configuration i))
                 (loop (++ i)))))
    
    ; Evolve the CA
    
    (let loop ((i 0)
               (tmp-ca ca))
      (if (< i maximum-ca-iterations)
          (let ((new-ca (ca-iterate ca)))
            (if (vector-compare (ca-grid tmp-ca) (ca-grid new-ca)) ; fixed configuration?
                (let ((n-ones (vector-count-ocurrences (ca-grid tmp-ca) 1 =)))
                  (cond ((and (= n-ones 0) (not density>0.5?)) 1) ; classification ok
                        ((and (= n-ones size) density>0.5?) 1) ; classification ok
                        (else (partial-credit n-ones density>0.5?)))); classification wrong
                (loop (++ i)
                      new-ca)))
          0))))

(define (make-density-genome-evaluator width test-set-size maximum-ca-iterations)
  (let* ((training-set (training-set-make width test-set-size))
         (density-gt-0.5?
          (map (lambda (x) (> x 0.5))
               (map density-of-1s training-set))))
    (lambda (genome)
      (let* ((rule (genome->rule genome))
             (ca (make-ca (list rule) width 3)))
        (let loop ((initial-configurations training-set)
                   (expected-results density-gt-0.5?)
                   (score 0))
          (if (not (null? initial-configurations))
              (loop (cdr initial-configurations)
                    (cdr expected-results)
                    (+ score (test-rule ca
                                        maximum-ca-iterations
                                        (car initial-configurations)
                                        (car expected-results))))
              score))))))


; *********************************
;
; Use the genetic algorithm with our task
;
; *********************************

;(define N 149) ; CA width
;(define M 320) ; maximum ca iterations
;(define SAMPLES 100) ; How many samples for the fitness function
;(define POP-SIZE 100)


(define N 51) ; CA width
(define M 150) ; maximum ca iterations
(define SAMPLES 100) ; How many samples for the fitness function
(define POP-SIZE 100)

(define the-evaluator #f) ; the evaluator
(define reset-evaluator! #f) ; reset the evaluator

(define (set-evaluator!)
  (define (evaluator-make) (make-density-genome-evaluator N SAMPLES M))
  (define e (evaluator-make))
  
  (set! the-evaluator (lambda (genome) (exact->inexact (/ (e genome) SAMPLES))))
  (set! reset-evaluator! (lambda () (set! e (evaluator-make)))))

(set-evaluator!)

(define ga (ga-make POP-SIZE 128 genome-uniform-initializer! the-evaluator #t 0.8 0.05 0.8))

(define (step n)
  (let loop ((i n))
    (if (> i 0)
        (begin
          (write (list-sort (map car (ga 'pop)) >))
          (reset-evaluator!)
          (ga-iterate! ga 1)
          (newline)
          (loop (-- i))))))

; (step 100)

; TODO! Change the evaluator after each test!

(define (make-tests)
  (define (test-1 test)
    (if (test) "ok" "FAILED"))
  (define available-tests
    (list test-vector-fill-random
          test-training-set-make))
  
  (map test-1 available-tests))
