;;; Получает n-ное число Фибоначчи
(defun fib (num) 
  (cond ((= num 0) 0)
        ((= num 1) 1)
        ((+ (fib (- num 2)) 
            (fib (- num 1)) ))))

;;; Генерирует последовательность натуральных чисел от 1 до len включительно.
(defun seq (len)
  ((defun seq-impl (len cdr-val)
     (if (<= len 0) cdr-val
       (seq-impl (- len 1) (cons len cdr-val)))) len NIL))

;;; Применяет функцию к каждому элементу списка
(defun map (func lst)
  (if lst (cons (func (car lst)) 
                (map func (cdr lst)) )))

(print (map fib (seq 50)))
