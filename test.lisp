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

'fib
(fib 2)
(fib 3)
(fib 4)
(fib 5)
(fib 6)
(fib 7)

'pascal
(pascal 4 0)
(pascal 4 1)
(pascal 4 2)
(pascal 4 3)
(pascal 4 4)

'fib
(fib 1000)
'pascal
(pascal 1000 500)
(pascal 800 500)

