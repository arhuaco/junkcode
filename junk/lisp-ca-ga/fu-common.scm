; Jan 2006
; Nelson Castillo
; http://arhuaco.org
;

(module fu-common mzscheme

  (provide ++ -- inc! dec! vector-compare vector-count-ocurrences vector-duplicate accumulate list-average
           random-real list-sort assert vector-split)
        
  (require (lib "list.ss" "mzlib")) ; mergesort


; *********************************
;
; Auxiliar functions
;
; *********************************

  (define (assert is-true? msg)
    (if (not is-true?)
        (begin (display msg)
        (exit))))

  (define list-sort mergesort)

  (define-syntax ++   ; Read-only increment
    (syntax-rules ()
      ((++ x) (+ x 1))))

  (define-syntax --   ; Read-only decrement
    (syntax-rules ()
      ((-- x) (- x 1))))

  (define-syntax inc! ; Mutable increment
    (syntax-rules ()
      ((inc! x) (set! x (+ 1 x)))))

  (define-syntax dec! ; Mutable decrement
    (syntax-rules ()
      ((dec! x) (set! x (- x 1)))))

  (define (vector-duplicate sample)
    (let* ((len (vector-length sample))
           (new (make-vector len)))
      (let loop ((i 0))
        (if (< i len)
            (begin (vector-set! new i (vector-ref sample i))
                   (loop (++ i)))
            new))))

  (define (vector-compare v1 v2)
    (assert (= (vector-length v1)
               (vector-length v2))
            "vectors must have the same size in vector-compare")
    (let ((len (vector-length v1)))
      (let loop ((i 0))
        (if (< i len)
            (if (= (vector-ref v1 i) (vector-ref v2 i))
                (loop (++ i))
                #f)
            #t))))

  (define (vector-split v n-parts)
    (assert (= (modulo (vector-length v) n-parts) 0)
            "vector cannot be split in n-parts parts")
    (let loop ((i 0)
               (list null))
      (if (< i n-parts)
          (loop (++ i)
                (let* ((new-size (/ (vector-length v) n-parts))
                       (new (make-vector new-size))
                       (offset (* i new-size)))
                  (let fill-vector ((j 0))
                    (if (< j new-size)
                        (begin
                          (vector-set! new j (vector-ref v (+ offset j)))
                          (fill-vector (++ j)))
                        (cons new list)))))
          (reverse list))))

  
; Accumulation pattern

  (define (accumulate initial-value operation list)
    (let loop ((total initial-value)
               (operands list))
      (if (null? operands)
          total
          (loop (operation total (car operands))
                (cdr operands)))))

  (define (list-average l)
    (let ((sums (accumulate (cons 0 0)
                            (lambda (x y)
                              (cons (+ (car x) y) (+ 1 (cdr x))))
                            l)))
      (/ (car sums) (cdr sums))))

  (define (vector-count-ocurrences v object equal-op)
    (let ((size (vector-length v)))
      (let loop ((i 0)
                (count 0))
        (if (< i size)
          (let ((sum (if (equal-op object (vector-ref v i))
                               1
                               0)))
            (loop (++ i)
                  (+ count sum)))
          count))))
    

; TODO: learn about http://srfi.schemers.org/srfi-27/srfi-27.html
; TODO: find out how to use a better PRNG.

  (define (random-real)
    (/ (random 275604540) 275604541.0)))
