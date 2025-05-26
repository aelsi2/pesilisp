(defun fib (num) (if (= num 0) 0 (if (= num 1) 1 (+ (fib (- num 2)) (fib (- num 1))))))
(defun pascal (n k) (if (or (= n k) (= k 0)) 1 (+ (pascal (- n 1) (- k 1)) (pascal (- n 1) k))))
