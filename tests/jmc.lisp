(define null
  (lambda (x)
    (eq x (quote ()))))

(define and
  (lambda (x y)
    (cond (x (cond (y (quote #t)) ((quote #t) (quote ()))))
          (else (quote ())))))

(define not
  (lambda (x)
    (cond (x (quote ()))
          (else (quote #t)))))

(define append
  (lambda (x y)
    (cond ((null x) y)
          (else (cons (car x) (append (cdr x) y))))))

(quote andtest)
(and (atom (quote a)) (eq (quote a) (quote a)))
(quote nottest)
(not (eq (quote a) (quote a)))
(not (eq (quote a) (quote b)))