; Transform a line consisting of a character followed by a space and a number
; into a cons of the character and the number.  For example, "a 23" is turned
; into (#\a 23).  Note that it receives a stream of characters rather than a
; string.


(define (counts-line line)
  (cons (stream-car line) (stream->number (stream-cddr line))))

; Given a path for the keystrokes, return a stream with the values of the form
; (char . count), where char is a character and count an integer.

(define counts->stream
  (compose
    (cut stream-map counts-line <>)
    stream-cdr
    stream-lines
    port->stream
    open-input-file))

; Given a path for the keystrokes, return a vector with the counts for each
; character.  The characters are indexed according to their byte value
; (obtained through char->integer).

(define (counts->vector file)
  (let ((counts (make-vector 255 0)))
    (stream-for-each
      (lambda (data)
        (vector-set! counts (char->integer (car data)) (cdr data)))
      (counts->stream file))
    counts))
