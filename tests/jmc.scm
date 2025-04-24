(define car
  (lambda (x)
    (car x)))

(define cdr
  (lambda (x)
    (cdr x)))

(define caar
  (lambda (x)
    (car (car x))))

(define cadr
  (lambda (x)
    (car (cdr x))))

(define cdar
  (lambda (x)
    (cdr (car x))))

(define cddr
  (lambda (x)
    (cdr (cdr x))))

(define caaar
  (lambda (x)
    (car (car (car x)))))

(define caadr
  (lambda (x)
    (car (car (cdr x)))))

(define cadar
  (lambda (x)
    (car (cdr (car x)))))

(define caddr
  (lambda (x)
    (car (cdr (cdr x)))))

(define cdaar
  (lambda (x)
    (cdr (car (car x)))))

(define cdadr
  (lambda (x)
    (cdr (car (cdr x)))))

(define cddar
  (lambda (x)
    (cdr (cdr (car x)))))

(define cdddr
  (lambda (x)
    (cdr (cdr (cdr x)))))

(define caaaar
  (lambda (x)
    (car (car (car (car x))))))

(define caaadr
  (lambda (x)
    (car (car (car (cdr x))))))

(define caadar
  (lambda (x)
    (car (car (cdr (car x))))))

(define caaddr
  (lambda (x)
    (car (car (cdr (cdr x))))))

(define cadaar
  (lambda (x)
    (car (cdr (car (car x))))))

(define cadadr
  (lambda (x)
    (car (cdr (car (cdr x))))))

(define caddar
  (lambda (x)
    (car (cdr (cdr (car x))))))

(define cadddr
  (lambda (x)
    (car (cdr (cdr (cdr x))))))

(define cdaaar
  (lambda (x)
    (cdr (car (car (car x))))))

(define cdaadr
  (lambda (x)
    (cdr (car (car (cdr x))))))

(define cdadar
  (lambda (x)
    (cdr (car (cdr (car x))))))

(define cdaddr
  (lambda (x)
    (cdr (car (cdr (cdr x))))))

(define cddaar
  (lambda (x)
    (cdr (cdr (car (car x))))))

(define cddadr
  (lambda (x)
    (cdr (cdr (car (cdr x))))))

(define cdddar
  (lambda (x)
    (cdr (cdr (cdr (car x))))))

(define cddddr
  (lambda (x)
    (cdr (cdr (cdr (cdr x))))))

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

(define assoc
  (lambda (x y)
    (cond ((eq (caar y) x) (cadar y))
          (else (assoc x (cdr y))))))

(quote andtest)
(and (atom (quote a)) (eq (quote a) (quote a)))
(quote nottest)
(not (eq (quote a) (quote a)))
(not (eq (quote a) (quote b)))