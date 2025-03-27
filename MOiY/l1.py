import numpy as np

def collect_data():
    try:
        n = int(input("Введите размерность матриц: "))
        if n <= 0:
            raise ValueError("Размерность должна быть положительным числом.")

        def read_matrix(prompt):
            print(prompt)
            matrix = []
            for row_num in range(n):
                while True:
                    row = input(f"Строка {row_num + 1}: ").strip().split()
                    if len(row) == n:
                        try:
                            matrix.append([float(val) for val in row])
                            break
                        except ValueError:
                            print("Ошибка: введите числовые значения.")
                    else:
                        print(f"Ошибка: должно быть {n} чисел в строке.")
            return np.array(matrix, dtype=float)

        A = read_matrix("Введите элементы матрицы A:")
        A_inv = read_matrix("Введите элементы обратной матрицы A⁻¹:")
        
        print("Введите элементы вектора x:")
        while True:
            x_input = input(f"x: ").strip().split()
            if len(x_input) == n:
                try:
                    x = np.array([float(val) for val in x_input])
                    break
                except ValueError:
                    print("Ошибка: введите числовые значения.")
            else:
                print(f"Ошибка: должно быть {n} элементов.")

        while True:
            try:
                i = int(input(f"Введите индекс столбца для замены (от 1 до {n}): "))
                if 1 <= i <= n:
                    break
                else:
                    print(f"Ошибка: индекс должен быть от 1 до {n}.")
            except ValueError:
                print("Ошибка: введите целое число.")
        
        return A, A_inv, x, i

    except ValueError as e:
        print(f"Ошибка: {e}")
        exit(1)

def fix_zero_values(matrix):
   return np.where(np.isclose(matrix, 0), 0.0, matrix)

def calculate_inverse(A, A_inv, x, i):
   
    A_prime = A.copy()
    A_prime[:, i - 1] = x
    fix_zero_values(A_prime)
    
    print("Шаг 1: Находим вектор l")
    l = np.dot(A_inv, x)
    print(f"l = A^(-1) * x =\n{fix_zero_values(l.reshape(-1, 1))}")
    
    if l[i - 1] == 0:
        print(f"l[{i}] = {l[i - 1]}: Матрица A' необратима.")
        return None, None, None
    
    print(f"l[{i}] != 0: Матрица A' обратима.")
    
    print("Шаг 2: Формируем вектор ~l")
    l_e = l.copy()
    l_e[i - 1] = -1
    print(f"~l =\n{fix_zero_values(l_e.reshape(-1, 1))}")
    
    print(f"Шаг 3: Находим вектор ~l = -1 /l_i[{i}] * ~l")
    l_i = (-1 / l[i - 1]) * l_e
    print(f"l_i =\n{fix_zero_values(l_i.reshape(-1, 1))}")
    
    print("Шаг 4: Формируем матрицу Q")
    Q = np.eye(A.shape[0])
    Q[:, i - 1] = l_i
    print(f"Матрица Q:\n{fix_zero_values(Q)}")
    
    print("Шаг 5: Находим A'^(-1) = Q * A^(-1)")
    A_inv_new = np.dot(Q, A_inv)
    print(f"(A')^(-1) = Q * A^(-1) =\n{fix_zero_values(A_inv_new)}")
    
    return A_inv_new, Q, A_prime

def main():
    try:
        A, A_inv, x, i = collect_data()
        A_inv_new, Q, A_prime = calculate_inverse(A, A_inv, x, i)
        if A_inv_new is not None:
            fix_zero_values(A_inv_new)
    except Exception as e:
        print(f"Ошибка: {e}")

if __name__ == "__main__":
    main()
