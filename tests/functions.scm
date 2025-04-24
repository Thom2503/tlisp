(define null (lambda (x) (eq x (quote ()))))
(null (quote a))
(null (quote ()))