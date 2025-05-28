;;; Получает k-й элемент n-й строки треугольника Паскаля 
(defun pascal (n k) 
  (if (or (= n k) (= k 1)) 1 
    (+ (pascal (- n 1) (- k 1)) 
       (pascal (- n 1) k))))

;;; Генерирует последовательность натуральных чисел от 1 до len включительно.
(defun seq (len)
  ((defun seq-impl (len cdr-val)
     (if (<= len 0) cdr-val
       (seq-impl (- len 1) (cons len cdr-val)))) len NIL))

;;; Применяет функцию к каждому элементу списка
(defun map (func lst)
  (if lst (cons (func (car lst)) 
                (map func (cdr lst)) )))

;;; Получает n-ю строку треугольника Паскаля.
(defun pascal-row (n)
  (map (lambda (k) (pascal n k)) (seq n)))

(let
  ((print-row (lambda (n) (print (pascal-row n)))))
  (map print-row (seq 20)))
