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

(define list
  (lambda (x y)
    (cons x (cons y (quote ())))))

(define pair
  (lambda (x y)
    (cond ((and (null x) (null y)) (quote ()))
          ((and (not (atom x)) (not (atom y)))
           (cons (list (car x) (car y))
                 (pair (cdr x) (cdr y)))))))

(define assoc
  (lambda (x y)
    (cond ((eq (car (car y)) x) (car (cdr (car (cdr y)))))
          (else (assoc x (cdr y))))))

(define eval
  (lambda (e a)
    (cond
      ((atom e) (assoc e a))
      ((atom (car e))
       (cond
         ((eq (car e) (quote quote)) (car (cdr e)))
         ((eq (car e) (quote atom)) (atom (eval (car (cdr e)) a)))
         ((eq (car e) (quote eq)) (eq (eval (car (cdr e)) a)
                                      (eval (car (cdr (cdr e))) a)))
         ((eq (car e) (quote car)) (car (eval (car (cdr e)) a)))
         ((eq (car e) (quote cdr)) (cdr (eval (car (cdr e)) a)))
         ((eq (car e) (quote cons)) (cons (eval (car (cdr e)) a)
                                          (eval (car (cdr (cdr e))) a)))
         ((eq (car e) (quote cond)) (evcon (cdr e) a))
         (else (eval (cons (assoc (car e) a) (cdr e)) a))))
      ((eq (car (car e)) (quote label))
       (eval (cons (car (cdr (cdr (car e)))) (cdr e))
             (cons (list (car (cdr (car e))) (car e)) a)))
      ((eq (car (car e)) (quote lambda))
       (eval (car (cdr (cdr (car e))))
             (append (pair (car (cdr (car e)))
                           (evlis (cdr e) a))
                     a))))))

(define evcon
  (lambda (c a)
    (cond ((eval (car (car c)) a)
           (eval (car (cdr (car c))) a))
          (else (evcon (cdr c) a)))))

(define evlis
  (lambda (m a)
    (cond ((null m) (quote ()))
          (else (cons (eval (car m) a)
                      (evlis (cdr m) a))))))

(eval (quote (eq (quote a) (quote a))) (quote ()))