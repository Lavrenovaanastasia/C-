import numpy as np

def clean_matrix(matrix):
    cleaned = matrix.copy()
    cleaned[np.isclose(cleaned, 0.0)] = 0.0
    return cleaned

def print_iteration(phase, iteration, AB, AB_inv, cB, u, delta, j0=None, z=None, theta=None, theta0=None, k=None, j_star=None, B_new=None, x_new=None,
):
    print(f"-------------Итерация {iteration}----------------")
    print("Шаг 1. Строим базисную матрицу A и находим её обратную:")
    print("A:")
    print(clean_matrix(AB))
    print("(A)^(-1):")
    print(clean_matrix(AB_inv))
    print("Шаг 2. Формируем вектор cB:")
    print(f" cB: {cB}")    
    print("Шаг 3. Находим вектор потенциалов:")
    print(f" u: {u}")    
    print("Шаг 4. Находим вектор оценок:")
    print(f" Δ: {delta}") 
    if j0 is not None:
        print("Шаг 6. Выбираем входящую переменную:")
        print(f"j_0: {j0 + 1}")
    if z is not None:
        print("Шаг 7. Вычисляем вектор z:")
        print(f" z: {z}")
    if theta is not None:
        print("Шаг 8. Вычисляем вектор Q:")
        print(f" Q: {theta}")
    if theta0 is not None:
        print("Шаг 9. Определяем минимальное Q_0:")
        print(f"Q_0 = {theta0}")
    if k is not None and j_star is not None:
        print("Шаг 11. Выбираем покидающую переменную:")
        print(f"k: {k + 1}")
        print(f"j* = {j_star + 1}")
    if B_new is not None:
        print("Шаг 12. Обновляем базис:")
        B_new_int = [int(b + 1) for b in B_new]
        print(f"B: {B_new_int}")
    if x_new is not None:
        print("Шаг 13. Обновляем план x:")
        print(f"x: {x_new}")

def simplex_main_phase_with_basis(
    c, A, x_initial, B_initial, phase_label="Основная фаза"
):
    c = np.array(c, dtype=float) 
    A = np.array(A, dtype=float) 
    x = np.array(x_initial, dtype=float)  
    B = [b_idx - 1 for b_idx in B_initial]  
    
    m, n = A.shape 
    iteration = 1  

    while True:  
        AB = A[:, B]
        try:
            AB_inv = np.linalg.inv(AB)  # Обратная базисная матрица
        except np.linalg.LinAlgError:
            print("Ошибка: Базисная матрица AB необратима.")
            return None, None, None

        # Вычисляем коэффициенты целевой функции для базисных переменных
        cB = c[B]
        u = cB @ AB_inv  #  умножение
        delta = u @ A - c 
        print_iteration(phase_label, iteration, AB, AB_inv, cB, u, delta)

        # Если все элементы delta >= 0, найден оптимальный план
        if np.all(delta >= 0):
            return x, B, AB_inv 

        j0_candidates = np.where(delta < 0)[0] 
        if len(j0_candidates) == 0:
            return x, B, AB_inv  

        j0 = j0_candidates[0] 
        Aj0 = A[:, j0]  
        z = AB_inv @ Aj0  

        theta = [x[B[i]] / z[i] if z[i] > 0 else np.inf for i in range(m)]
        theta0 = np.min(theta) 
        if theta0 == np.inf:
            print("Ошибка: Функция не ограничена.") 
            return None, None, None
        # Определяем индекс переменной
        k = np.where(np.array(theta) == theta0)[0][0]
        j_star = B[k]  # Индекс переменной, которая выходит из базиса
        B_new = B.copy()  # Создаем новую версию базиса
        B_new[k] = j0  
        x_new = x.copy()
        x_new[j0] = theta0 
        for i in range(m):
            if i != k:
                x_new[B[i]] -= theta0 * z[i]  
        x_new[j_star] = 0 

        print_iteration(phase_label, iteration, AB, AB_inv, cB, u, delta, j0=j0, z=z, theta=theta, theta0=theta0, k=k, j_star=j_star, B_new=B_new, x_new=x_new)
        B = B_new
        x = x_new
        iteration += 1 

def simplex_initial_phase(c, A, b):
    A = np.array(A, dtype=float)
    b = np.array(b, dtype=float)
    m, n = A.shape

    for i in range(m):
        if b[i] < 0:
            A[i, :] = -A[i, :]
            b[i] = -b[i]

    print(" Приведение b к неотрицательному виду")
    print("A:")
    print(clean_matrix(A))
    print("b:")
    print(b)

    c_aux = np.hstack((np.zeros(n), -np.ones(m)))
    A_e = np.hstack((A, np.eye(m)))
    x_e_initial = np.concatenate((np.zeros(n), b))
    B_aux_initial = [i for i in range(n + 1, n + m + 1)]

    print(" Формирование вспомогательной задачи")
    print("c^(¬T):")
    print(c_aux)
    print("A¬:")
    print(clean_matrix(A_e))
    print("x^(¬T):")
    print(x_e_initial)
    print("B:")
    print(B_aux_initial)

    print(" Решение вспомогательной задачи")
    x_e_opt, B_aux, AB_inv = simplex_main_phase_with_basis( c_aux, A_e, x_e_initial, B_aux_initial, phase_label="Вспомогательная фаза")
    if x_e_opt is None:
        print("Вспомогательная задача не имеет решения.")
        return None, None, None, None, None

    print("Оптимальное решение вспомогательной задачи:")
    print("x^T:")
    print(x_e_opt)
    print("B:")
    print([int(b) + 1 for b in B_aux])

    artificial_values = x_e_opt[n:]
    if not np.all(np.abs(artificial_values) < 1e-8):
        print("Ошибка: Искусственные переменные не обнулились. Допустимых планов нет.")
        return None, None, None, None, None

    x = x_e_opt[:n]
    print("Допустимый план для исходной задачи:")
    print("x:")
    print(x)

    while any(b_idx >= n for b_idx in B_aux):
        artificial_in_B = [b_idx for b_idx in B_aux if b_idx >= n]
        j_k = max(artificial_in_B)
        k = B_aux.index(j_k)
        i_row = j_k - n
        print(f"Корректировка: обнаружен искусственный базисный индекс {j_k + 1} (ограничение {i_row + 1}).")
        replaced = False
        for j in range(n):
            if j in B_aux:
                continue
            l_j = AB_inv @ A_e[:, j]
            if not np.isclose(l_j[k], 0.0):
                print(f" Заменяем {j_k + 1} на {j + 1} (l(j))[{k + 1}] = {l_j[k]:.4f}")
                B_aux[k] = j
                AB = A_e[:, B_aux]
                try:
                    AB_inv = np.linalg.inv(AB)
                except np.linalg.LinAlgError:
                    print("Ошибка: Базис после замены оказался необратим.")
                    return None, None, None, None, None
                replaced = True
                break
        if not replaced:
            print( f"j не принадлежат B удовлетворяют условию (l(j))[{k + 1}] = 0. Удаляем ограничение {i_row + 1}.")
            A = np.delete(A, i_row, axis=0)
            b = np.delete(b, i_row, axis=0)
            A_e = np.delete(A_e, i_row, axis=0)
            m -= 1
            del B_aux[k]
            if len(B_aux) > 0:
                AB = A_e[:, B_aux]
                try:
                    AB_inv = np.linalg.inv(AB)
                except np.linalg.LinAlgError:
                    print("Ошибка: Базис после удаления ограничений необратим.")
                    return None, None, None, None, None
            print("  Обновлённая матрица A:")
            print(clean_matrix(A))
            print("  Обновлённый вектор b:")
            print(b)
            print("  Новый базис B:")
            print([int(b) + 1 for b in B_aux])

    if m == 1 and np.isclose(b[0], 0.0):
        print("Корректировка: b[0] = 1")
        b[0] = 1

    B_final = [int(b_idx) + 1 for b_idx in B_aux]
    print(" Итоговый результат начальной фазы ")
    print("x:")
    print(x)
    print("B:")
    print(B_final)
    return x, B_aux, A, b, A_e

def get_vector(prompt, length, type_func=float):
    while True:
        try:
            values = input(prompt).strip().split()
            if len(values) != length:
                print(f"Введите ровно {length} элементов.")
                continue
            return [type_func(val) for val in values]
        except ValueError:
            print("Введите числовые значения.")

def get_matrix(prompt, m, n):
    print(prompt)
    matrix = []
    for i in range(1, m + 1):
        row = get_vector(f"Строка {i}: ", n)
        matrix.append(row)
    return matrix

def main():
    n = int(input("n: "))
    m = int(input("m: "))
    print(f"Введите вектор c:")
    c = get_vector("c: ", n)
    A = get_matrix(f"Введите матрицу A:", m, n)
    print(f"Введите вектор b:")
    b = get_vector("b: ", m)
    result = simplex_initial_phase(c, A, b)
    if result[0] is None:
        print("Допустимых планов нет.")
    else:
        x, B, A_new, b_new, A_e_new = result
        B_print = [int(idx) + 1 for idx in B]
        print("Ответ:")
        print("x^T = (", x, ")")
        print("B = {j_1 = ", B_print, "}")
        print("A = (", clean_matrix(A_new), ")")
        print("b = (", b_new, ")")

if __name__ == "__main__":
    main()