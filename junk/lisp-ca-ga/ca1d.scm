; This is the implementation for a binary unidimensional cellular automaton [1].
; The CA can have any size and radius.
; The CA can be uniform or non-uniform.
;
; Jan 2006
; Nelson Castillo
; http://arhuaco.org
; 
; [1] http://en.wikipedia.org/wiki/Cellular_automaton


; **************************************
;
; Configuration
; A configuration is a sequence of cells
;
; **************************************

(define (make-ca-grid width)
  (make-vector width))

(define (ca-grid-size grid)
  (vector-length grid))

(define (ca-grid-cell grid pos)
  (vector-ref grid pos))

(define (ca-grid-cell-set! grid pos new-value)
  (vector-set! grid pos new-value))

(define (ca-grid-fill-zero! grid)
  (vector-fill! grid 0))

(define (ca-grid-show grid)
  (display (list->string (map (lambda (x) (if (= x 0) #\space #\*))
                               (vector->list grid)))))


; Fill the grid with 0s and a 1 in the central cell

(define (grid-central-cell-initializer! grid) 
  (ca-grid-fill-zero! grid)
  (ca-grid-cell-set! grid (quotient (ca-grid-size grid)
                                    2)
                     1))

(define (grid-random-cell-initializer! grid) 
  (let loop ((i 0))
    (cond ((< i (ca-grid-size grid))
           (ca-grid-cell-set! grid i (random 2))
           (loop (+ i 1))))))

; **************************************
;
; Rule
; A rule is a procedure that computes the state of a cell at time t.
; It uses part of the configuration of time t-1 for this calculation.
;
; The integer rule follows the wolfram notation.
; **************************************

; (define (make-ca-rule integer-rule radius))

(define (make-rule int-rule radius) 
  
  (let* ((table-size (expt 2 (+ 1 (* radius 2))))
         (lookup-table (make-vector table-size)))
    
    (let fill-table ((rule int-rule) ; fill the lookup table
                     (pos 0))  
         (if (< pos table-size)
          (begin (vector-set! lookup-table
                              pos
                              (modulo rule 2))
                 (fill-table (quotient rule 2)
                             (+ pos 1)))))
    
    ; create the lookup procedure
    (lambda (ca next-cell-pos)
      (let loop ((cell-pos (+ next-cell-pos radius))
                 (power-of-2 1)
                 (lookup-pos 0))
        (if (< cell-pos (- next-cell-pos radius))
            (vector-ref lookup-table lookup-pos) ; resulting state
            (loop (- cell-pos 1)
                  (* power-of-2 2)
                  (+ lookup-pos
                     (* (ca-grid-cell (ca-grid ca) (modulo (+ (ca-grid-size (ca-grid ca)) 
                                                              cell-pos)
                                                           (ca-grid-size (ca-grid ca))))
                        power-of-2))))))))
                    
; **************************************
;
; Cellular Automaton
;
; The list of rules can consist of numbers or rules created
; with make-rule.
; **************************************

(define (make-ca the-rules width radius)
  (let ((grid (make-ca-grid width))
        (rules (if (number? (car the-rules))
                   (map (lambda (rule) (make-rule rule radius))
                        the-rules)
                   the-rules)))
    (cons grid (cons rules radius))))

(define ca-grid car)

(define ca-rules cadr)

(define ca-radius cddr)

(define (ca-iterate ca)
  (let loop ((new-ca (make-ca (ca-rules ca) (ca-grid-size (ca-grid ca)) (ca-radius ca)))
             ($cell 0)
             (rules (ca-rules ca)))
    (if (>= $cell (ca-grid-size (ca-grid ca)))
        new-ca
        (begin (ca-grid-cell-set! (ca-grid new-ca) $cell ((car rules) ca $cell))
               (loop  new-ca
                      (+ $cell 1)
                      (if (null? (cdr rules))
                          (ca-rules ca)
                          (cdr rules)))))))

; Evolve the CA

(define (ca-evolve rules width radius steps initialize-grid! show-grid)
  (let ((ca (make-ca rules width radius)))
    (initialize-grid! (ca-grid ca))
    (let loop ((i 0)
               (iter-ca ca))
      (if (< i steps)
          (begin (show-grid (ca-grid iter-ca))
                 (loop (+ i 1) 
                       (ca-iterate iter-ca)))))))

; Examples

(define (ca-show-examples)
  
  (define (show-header txt)
    (display txt)
    (newline))

  (show-header "rule 30 (radius 1)")

  (ca-evolve '(30)
              70
              1
              30
              grid-central-cell-initializer!
              (lambda (grid) (ca-grid-show grid) (newline)))
  
  (show-header "rule 2868868760 (radius 2)")
  
  (ca-evolve '(2868868760)
              70
              2
              40
              grid-central-cell-initializer!
              (lambda (grid) (ca-grid-show grid) (newline)))) 

(ca-show-examples)
