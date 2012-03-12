(use utils)

(define *counts* (make-vector 255 0))

;(define (read-count line)
;  (let ((line (read-line)))
;    (vector-set! *counts* (string-ref line 0)
;                          (string->number (substring line 2)))))

(define (read-count line)
  (vector-set! *counts* (string-ref line 0)
                          (string->number (substring line 2))))

(for-each-line read-count (open-input-file "/proc/keystrokes"))
