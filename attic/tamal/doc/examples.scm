; A few examples that work on Tamal
; Note that Tamal does not accept commented lines yet.
; So you'll have to copy and paste.

; WARNING : I haven't imlemented tail recursion, yet! So be
; nice and use small numbers for tests =)
; This is the next thing I'll try to do.

; The garbage colection is being done at the end of the process,
; but this is not the way to do it. One might want to run a process
; for ever.

; Reverse a list

(define (reverse-aux-add old-list new-list)
    (if (null? old-list)
        new-list
        (reverse-aux-add (cdr old-list) (cons (car old-list) new-list))))

(define (reverse l)
  (reverse-aux-add l '()))

(reverse '(1 2 3 hola dude (a b) 4 5 6))

; mapping on a tree
; Yes. I need to implement cond.
; and it's slow, mostly due to garbage collection issues I need to
; solve

(define (square-tree t)
  (if (null? t) '()
                (if (not (pair? t)) (* t t)
                                    (cons (square-tree (car t))
                                          (square-tree (cdr t))))))
(define tree '(1 2 3 (4 5) (6 7 (8 9))))

(square-tree tree)

; simple prime testing

(define square (lambda (x) (* x x)))

(define (smallest-divisor n)
  (find-divisor n 2))

(define (find-divisor n test-divisor)
   (if (> (square test-divisor) n) n
                                   (if (divides? test-divisor n) test-divisor
                                                                 (find-divisor n (+ test-divisor 1)))))
(define (divides? a b)
  (= (remainder b a) 0))

(define (prime? n)
  (= n (smallest-divisor n)))

(prime? 91)
(prime? 150)

; greatest common divisor

(define (gcd a b)
  (if (= b 0)
   a
   (gcd b (remainder a b))))

(gcd 78 50)

; A generic procedure for accumulations

(define (accumulate op initial sequence)
  (if (null? sequence)
      initial
      (op (car sequence)
      (accumulate op initial (cdr sequence)))))

(accumulate + 0 '(1 2 3 4 5))
(accumulate * 1 '(1 2 3 4 5))

; exponentiation in logaritmic time, I think.

(define (even? x)
  (if (= (remainder x 2) 0) #t #f))

(define (pow a b)
  (if (= b 0) 1
              (if (even? b) (* a a (pow a (- b 2)))
                            (* a (pow a (- b 1))))))
(pow 2 3)
(pow 14 15)

; from now on, things won't work. I have to fix this.

; A custom version of cons, car and cdr
; TODO: I have to define de function error
; and the string datatype to allow the user
; to signal errors

; This is not working! Fix!

(define (mycons x y)
  (define (magic selector)
    (if (= selector 0) x
                       (if (= selector 1) y
                                          error)))
  magic)

(define (mycar x)
  (x 0))

(define (mycdr x)
  (x 1))

(define pair (mycons 'a 'b))

(mycar pair)
(mycdr pair)

; church numerals

(define zero (lambda (f) (lambda (x) x)))

(define (add-1 n)
  (lambda (f) (lambda (x) (f ((n f) x)))))

(add-1 (add-1 zero))
