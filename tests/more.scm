(define and
  (lambda (x y)
    (cond (x (cond (y (quote #t)) ((quote #t) (quote ()))))
         ((quote #t) (quote ())))))
(and (atom (quote a)) (eq (quote a) (quote a)))
