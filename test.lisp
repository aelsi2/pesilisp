;;; Получает n-ное число Фибоначчи
(defun fib (num) 
  (if (= num 0) 0 
    (if (= num 1) 1 
      (+ (fib (- num 2)) 
         (fib (- num 1)) ))))

;;; Получает k-й элемент n-й строки треугольника Паскаля 
(defun pascal (n k) 
  (if (or (= n k) (= k 0)) 1 
    (+ (pascal (- n 1) (- k 1)) 
       (pascal (- n 1) k))))

;;; Читает список длиной len из stdin
(defun read-list (len) 
  (if (<= len 0) () 
    (cons (read) 
          (read-list (- len 1)) )))

(print 'fib)
(print (fib 2))
(print (fib 3))
(print (fib 4))
(print (fib 5))
(print (fib 6))
(print (fib 7))

(print 'pascal)
(print (pascal 4 0))
(print (pascal 4 1))
(print (pascal 4 2))
(print (pascal 4 3))
(print (pascal 4 4))

(print 'large-nums)
(print (fib 1000))
(print (pascal 1000 500))
(print (pascal 800 500))
