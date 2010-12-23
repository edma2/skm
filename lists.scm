(begin 
(define cons (lambda (car cdr) (lambda (c) (if (= c 'car) car (if (= c 'cdr) cdr '#f)))))
(define car (lambda (pair) (pair 'car))) 
(define cdr (lambda (pair) (pair 'cdr))))
