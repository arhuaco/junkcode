;
; In this file I compute the total nomber of CA I'd like to
; explore. It's just for reference.
;


(define (fact x)
  (if (<= x 0)
      1
      (* x (fact (- x 1)))))

; C(n,k) = n!/[k!(n-k)!]
; http://www.theory.csc.uvic.ca/~cos/inf/comb/CombinationsInfo.html

(define (C n k)
    (/ (fact n)
       (* (fact k)
          (fact (- n k)))))

(define (sum f start end)
  (if (> start end)
      0
      (+ (begin (write (f start)) (newline) (f start))
         (sum f
              (+ start 1)
              end))))

(define total (sum (lambda (x) (C 31 x))
                   0
                   32))

(write total)(newline)
