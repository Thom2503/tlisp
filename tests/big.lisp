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

(define length
  (lambda (x)
    (cond ((null x) (quote 0))
          (else (+ (quote 1) (length (cdr x)))))))

(define first
  (lambda (x)
    (car x)))

(define rest
  (lambda (x)
    (cdr x)))

(define sum
  (lambda (x)
    (cond ((null x) (quote 0))
          (else (+ (car x) (sum (cdr x)))))))

(define member?
  (lambda (x lst)
    (cond ((null lst) (quote ()))
          ((eq x (car lst)) (quote #t))
          (else (member? x (cdr lst))))))