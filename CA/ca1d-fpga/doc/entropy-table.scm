;
; We'd like to compute the entropy inside of the FPGA.
;
; This code can be run using Chicken Scheme

(require-extension format-modular)


(define M_LN2 0.69314718055994530942) ; math.h :)

; h bits
; timesteps
(define (entropy h timesteps)
  (lambda (counter)
    (if (= counter 0) 0
                      (let ((p (/ counter (/ timesteps h))))
                       (* p (/ (log p) (log 2)))))))

(define e1 (entropy 4 4096))

(define (show)
  (let loop ((i 0))
    (if (< i 1025)
        (begin (format #t "score(~a) = ~a  (minsq:~a) ~%" i (e1 i) (* (- 64 i)(- 64 i)))
               (loop (+ i 1))))))

(show)
